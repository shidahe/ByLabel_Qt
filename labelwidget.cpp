#include "labelwidget.h"
#include "labelimage.h"
#include <QKeyEvent>
#include <QtDebug>
#include "ED.h"
#include "edgeitem.h"

LabelWidget::LabelWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::gray);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
    pImage = NULL;
    kdtree = NULL;
    radiusNN = 10;
}

void LabelWidget::reset()
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

    if (pImage) {
        scene()->removeItem(pImage);
        delete pImage;
        pImage = NULL;
    }

    resetMatrix();
}

void LabelWidget::showImage(const cv::Mat& image)
{
    reset();

    pImage = new LabelImage(this, image);
    scene()->addItem(pImage);
    pImage->setPos(0,0);

    addEdges(image);
    buildKD();

    repaint();
}

void LabelWidget::addEdges(const cv::Mat &image)
{
    std::vector<std::list<cv::Point>> edges;
    ED::detectEdges(image, edges);
    for (const auto &edge : edges){
        if (edge.size() == 0) continue;
        EdgeItem* item = new EdgeItem(this, pImage, edge);
        pEdges.push_back(item);
        scene()->addItem(item);
        item->setPos(item->center());
    }
}

void LabelWidget::buildKD()
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

EdgeItem* LabelWidget::getNN(const QPointF& pos)
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

void LabelWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        setInteractive(false);
        setDragMode(ScrollHandDrag);
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    }
    else QGraphicsView::mousePressEvent(event);
}

void LabelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fake);
        setDragMode(NoDrag);
        setInteractive(true);
    }
    else QGraphicsView::mouseReleaseEvent(event);
}

#if QT_CONFIG(wheelevent)
void LabelWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void LabelWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}


