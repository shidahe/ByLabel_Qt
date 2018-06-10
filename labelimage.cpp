#include "labelimage.h"
#include "mat_qimage.h"
#include "edgeitem.h"
#include "endpoint.h"
#include <QGraphicsSceneHoverEvent>
#include "ED.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include "action.h"

LabelImage::LabelImage(LabelWidget *labelWidget, const cv::Mat& image)
    : parent(labelWidget)
{
    qimage = mat_to_qimage_ref(image);
    setZValue(-1);
    setAcceptHoverEvents(true);
//    setCacheMode(ItemCoordinateCache);

    pCurrEdge = NULL;
    kdtree = NULL;
    radiusNN = 10;
    maxActionListSize = 100;
}

LabelImage::~LabelImage()
{
    if (kdtree) {
        delete kdtree;
        kdtree = NULL;
    }
    ind2edge.clear();
    global2local.clear();
    edgePoints.clear();

    for (auto pEdge : pEdges)
        delete pEdge;
    pEdges.clear();

    pCurrEdge = NULL;
}

void LabelImage::addEdges(const cv::Mat &image)
{
    std::vector<std::list<cv::Point>> edges;
    ED::detectEdges(image, edges);
    for (const auto &edge : edges){
        if (edge.size() == 0) continue;
        EdgeItem* item = new EdgeItem(this, edge);
        pEdges.insert(item);
        scene()->addItem(item);
        item->setPos(item->center());
        item->createEndPoints();
    }
    buildKD();
}

void LabelImage::buildKD()
{
    edgePoints.clear();
    ind2edge.clear();
    if (kdtree) delete(kdtree);

    for (auto pEdge : pEdges) {
        edge2ind[pEdge] = edgePoints.size();
        for (int i = 0; i < (int)pEdge->points().size(); i++){
            auto point = pEdge->points()[i];
            cv::Point2f curr(point.x()+0.5, point.y()+0.5);
            edgePoints.push_back(curr);
            ind2edge[edgePoints.size()-1] = pEdge;
            global2local[edgePoints.size()-1] = i;
            pointMask[edgePoints.size()-1] = true;
        }
    }
    kdtree = new cv::flann::Index(cv::Mat(edgePoints).reshape(1), cv::flann::KDTreeIndexParams(1));

}

void LabelImage::searchNN(const QPointF& pos, EdgeItem*& pEdge, int& localIndex)
{
    pEdge = NULL;

    if (!kdtree) return;

    std::vector<float> query;
    query.push_back(pos.x());
    query.push_back(pos.y());

    std::vector<int> indices;
    std::vector<float> dists;
    // search for points in the circle
    kdtree->radiusSearch(query, indices, dists, pow(radiusNN,2), (int)(pow(radiusNN,2)*CV_PI));

    for (int i = 0; i < (int)indices.size(); i++) {
        if (pointMask[indices[i]] && dists[i] > 0) {
            pEdge = ind2edge[indices[i]];
            localIndex = global2local[indices[i]];
            break;
        }
    }

}

void LabelImage::searchNN(const QPointF& pos, EdgeItem*& pEdge)
{
    int temp;
    searchNN(pos, pEdge, temp);
}

void LabelImage::updateNNMask(EdgeItem* pEdge)
{
    for (int i = 0; i < (int)pEdge->points().size(); i++) {
        if (pEdge->pointVisible(i)) {
            pointMask[edge2ind[pEdge]+i] = true;
        } else {
            pointMask[edge2ind[pEdge]+i] = false;
        }
    }
}

QRectF LabelImage::boundingRect() const
{
    return QRectF(-qimage.width()/2.0, -qimage.height()/2.0, qimage.width(), qimage.height());
}

void LabelImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QRectF target(-qimage.width()/2.0, -qimage.height()/2.0, qimage.width(), qimage.height());
    QRectF source(0.0, 0.0, qimage.width(), qimage.height());

    painter->drawImage(target, qimage, source);
}

void LabelImage::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = item2image(event->lastPos());
    EdgeItem* prev;
    searchNN(lastPos, prev);
    QPointF pos = item2image(event->pos());
    EdgeItem* curr;
    int localIndex;
    searchNN(pos, curr, localIndex);
    if(prev && prev != curr) prev->hoverLeave();
    pCurrEdge = curr;
    if(curr) curr->hoverEnter(pos, localIndex);
    QGraphicsObject::hoverMoveEvent(event);
}

void LabelImage::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pos = item2image(event->pos());
    EdgeItem* curr;
    int localIndex;
    searchNN(pos, curr, localIndex);
    pCurrEdge = curr;
    if(curr) curr->hoverEnter(pos, localIndex);
    QGraphicsObject::hoverEnterEvent(event);
}

void LabelImage::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = item2image(event->lastPos());
    EdgeItem* prev;
    searchNN(lastPos, prev);
    pCurrEdge = NULL;
    if(prev) prev->hoverLeave();
    QGraphicsObject::hoverLeaveEvent(event);
}

void LabelImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (pCurrEdge) {
        Action* act = new SelectEdge(pCurrEdge);
        act->perform();
        addAction(act);
    }
    QGraphicsObject::mousePressEvent(event);
}

QPointF LabelImage::image2item(const QPointF &pos)
{
    return pos + boundingRect().topLeft();
}

QPointF LabelImage::item2image(const QPointF &pos)
{
    return pos - boundingRect().topLeft();
}

EdgeItem* LabelImage::currEdge()
{
    return pCurrEdge;
}

void LabelImage::splitEdge()
{
    if (pCurrEdge && pCurrEdge->showingSplit()) {
        SplitEdge* pAction = new SplitEdge(this, pCurrEdge);
        pAction->perform();
        addAction(pAction);
        pCurrEdge = NULL;
    }
}

void LabelImage::performSplitEdge(EdgeItem* oldEdge, EdgeItem* newEdge1, EdgeItem* newEdge2)
{
    scene()->addItem(newEdge1);
    newEdge1->setPos(newEdge1->center());
    newEdge1->createEndPoints(oldEdge->head()->indexOnEdge(), (int)(newEdge1->points().size())-1);

    scene()->addItem(newEdge2);
    newEdge2->setPos(newEdge2->center());
    newEdge2->createEndPoints(0, oldEdge->tail()->indexOnEdge() - (int)(newEdge1->points().size()));

    // update pEdges
    pEdges.erase(oldEdge);
    pEdges.insert(newEdge1);
    pEdges.insert(newEdge2);

    // update ind2edge and global2local
    for (int i = 0; i < (int)(oldEdge->points().size()); i++){
        if (i < (int)(newEdge1->points().size())){
            ind2edge[edge2ind[oldEdge]+i] = newEdge1;
        } else {
            ind2edge[edge2ind[oldEdge]+i] = newEdge2;
            global2local[edge2ind[oldEdge]+i] = i - (int)(newEdge1->points().size());
        }
    }

    // update edge2ind
    edge2ind[newEdge1] = edge2ind[oldEdge];
    edge2ind[newEdge2] = edge2ind[oldEdge] + (int)(newEdge1->points().size());
    edge2ind.erase(oldEdge);

    // remove old edge, keep it in memory in case of reverse action
    oldEdge->hoverLeave();
    oldEdge->removeFromScene();
}

void LabelImage::reverseSplitEdge(EdgeItem* oldEdge, EdgeItem* newEdge1, EdgeItem* newEdge2)
{
    scene()->addItem(oldEdge);
    oldEdge->setPos(oldEdge->center());
    oldEdge->createEndPoints(newEdge1->head()->indexOnEdge(),
                             (int)(newEdge1->points().size()) + newEdge2->tail()->indexOnEdge());

    // update pEdges
    pEdges.erase(newEdge1);
    pEdges.erase(newEdge2);
    pEdges.insert(oldEdge);

    // update ind2edge and global2local
    for (int i = 0; i < (int)(newEdge1->points().size()); i++) {
        ind2edge[edge2ind[newEdge1]+i] = oldEdge;
    }
    for (int i = 0; i < (int)(newEdge2->points().size()); i++) {
        ind2edge[edge2ind[newEdge2]+i] = oldEdge;
        global2local[edge2ind[newEdge2]+i] = i + (int)(newEdge1->points().size());
    }

    // update edge2ind
    edge2ind[oldEdge] = edge2ind[newEdge1];
    edge2ind.erase(newEdge1);
    edge2ind.erase(newEdge2);

    // remove edge but keep in memory
    newEdge1->hoverLeave();
    newEdge2->hoverLeave();
    newEdge1->removeFromScene();
    newEdge2->removeFromScene();
}

void LabelImage::addAction(Action* act)
{
    if (actionList.size() >= maxActionListSize) {
        Action* temp = actionList.back();
        actionList.pop_back();
        delete temp;
    }
    actionList.push_front(act);

    for (auto pAction : redoList)
        delete pAction;
    redoList.clear();
}

void LabelImage::reverseAction()
{
    if (actionList.size()) {
        Action* act = actionList.front();
        act->reverse();
        actionList.pop_front();
        redoList.push_back(act);
    }
}

void LabelImage::redoAction()
{
    if (redoList.size()) {
        Action* act = redoList.back();
        act->perform();
        redoList.pop_back();
        actionList.push_front(act);
    }
}


