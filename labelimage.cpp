#include "labelimage.h"
#include "mat_qimage.h"
#include "edgeitem.h"
#include <QGraphicsSceneHoverEvent>
#include "ED.h"

LabelImage::LabelImage(LabelWidget *labelWidget, const cv::Mat& image)
    : parent(labelWidget)
{
    qimage = mat_to_qimage_ref(image);
    setZValue(-1);
    setAcceptHoverEvents(true);
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
    edgePoints.clear();

    for (auto pEdge : pEdges)
        delete pEdge;
    pEdges.clear();
}

void LabelImage::addEdges(const cv::Mat &image)
{
    std::vector<std::list<cv::Point>> edges;
    ED::detectEdges(image, edges);
    for (const auto &edge : edges){
        if (edge.size() == 0) continue;
        EdgeItem* item = new EdgeItem(this, edge);
        pEdges.push_back(item);
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
        for (auto point : pEdge->points()){
            cv::Point2f curr(point.x()+0.5, point.y()+0.5);
            edgePoints.push_back(curr);
            ind2edge[edgePoints.size()-1] = pEdge;
        }
    }
    kdtree = new cv::flann::Index(cv::Mat(edgePoints).reshape(1), cv::flann::KDTreeIndexParams());
}

EdgeItem* LabelImage::getNN(const QPointF& pos)
{
    if (!kdtree) return NULL;

    std::vector<float> query;
    query.push_back(pos.x());
    query.push_back(pos.y());

    std::vector<int> indices;
    std::vector<float> dists;
    kdtree->radiusSearch(query, indices, dists, pow(radiusNN,2), 1);

//    qDebug() << query;
//    qDebug() << indices << edgePoints.size() << ind2edge.size();
//    qDebug() << dists;
//    qDebug() << edgePoints[indices[0]].x << edgePoints[indices[0]].y;

//    if (indices.size() != 1 || dists.size() != 1) return NULL;
//    if (dists[0] <= 0 || dists[0] > pow(radiusNN,2)) return NULL;
//    if (ind2edge.find(indices[0]) == ind2edge.end()) return NULL;

    if (dists[0] <= 0) return NULL;

    return ind2edge[indices[0]];
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
    QPointF lastPos = event->lastPos() - boundingRect().topLeft();
    EdgeItem* prev = getNN(lastPos);
    if(prev) prev->hoverLeave();
    QPointF pos = event->pos() - boundingRect().topLeft();
    EdgeItem* curr = getNN(pos);
    if(curr) curr->hoverEnter();
}

void LabelImage::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pos = event->pos() - boundingRect().topLeft();
    EdgeItem* curr = getNN(pos);
    if(curr) curr->hoverEnter();
}

void LabelImage::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = event->lastPos() - boundingRect().topLeft();
    EdgeItem* prev = getNN(lastPos);
    if(prev) prev->hoverLeave();
}

QPointF LabelImage::image2item(const QPointF &pos)
{
    return pos + boundingRect().topLeft();
}

QPointF LabelImage::item2image(const QPointF &pos)
{
    return pos - boundingRect().topLeft();
}

