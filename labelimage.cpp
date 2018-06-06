#include "labelimage.h"
#include "mat_qimage.h"
#include "edgeitem.h"
#include "endpoint.h"
#include <QGraphicsSceneHoverEvent>
#include "ED.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTime>

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
    QGraphicsItem::hoverMoveEvent(event);
}

void LabelImage::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pos = item2image(event->pos());
    EdgeItem* curr;
    int localIndex;
    searchNN(pos, curr, localIndex);
    pCurrEdge = curr;
    if(curr) curr->hoverEnter(pos, localIndex);
    QGraphicsItem::hoverEnterEvent(event);
}

void LabelImage::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = item2image(event->lastPos());
    EdgeItem* prev;
    searchNN(lastPos, prev);
    pCurrEdge = NULL;
    if(prev) prev->hoverLeave();
    QGraphicsItem::hoverLeaveEvent(event);
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
    QTime myTime;
    myTime.start();

    qDebug() << "splitting edge";

    if (pCurrEdge && pCurrEdge->showingSplit()) {
        std::vector<EdgeItem*> newEdges = pCurrEdge->split();
        if (newEdges.size() != 2) return;

        scene()->addItem(newEdges[0]);
        newEdges[0]->setPos(newEdges[0]->center());
        newEdges[0]->createEndPoints(pCurrEdge->head()->indexOnEdge(), (int)(newEdges[0]->points().size())-1);

        scene()->addItem(newEdges[1]);
        newEdges[1]->setPos(newEdges[1]->center());
        newEdges[1]->createEndPoints(0, pCurrEdge->tail()->indexOnEdge() - (int)(newEdges[0]->points().size()));

        // update pEdges
        pEdges.erase(pCurrEdge);
        pEdges.insert(newEdges[0]);
        pEdges.insert(newEdges[1]);

        // update ind2edge and global2local
        for (int i = 0; i < (int)(pCurrEdge->points().size()); i++){
            if (i < (int)(newEdges[0]->points().size())){
                ind2edge[edge2ind[pCurrEdge]+i] = newEdges[0];
            } else {
                ind2edge[edge2ind[pCurrEdge]+i] = newEdges[1];
                global2local[edge2ind[pCurrEdge]+i] = i - (int)(newEdges[0]->points().size());
            }
        }

        // update edge2ind
        edge2ind[newEdges[0]] = edge2ind[pCurrEdge];
        edge2ind[newEdges[1]] = edge2ind[pCurrEdge] + (int)(newEdges[0]->points().size());
        edge2ind.erase(pCurrEdge);

        // remove old edge, keep it in memory in case of reverse action
        pCurrEdge->removeFromScene();
        pCurrEdge = NULL;
    }

    qDebug() << myTime.elapsed();
}

