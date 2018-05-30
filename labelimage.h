#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QGraphicsItem>
#include <QImage>
#include "labelwidget.h"
#include <set>
#include <opencv2/flann/miniflann.hpp>

class EndPoint;

class LabelImage : public QGraphicsItem
{
public:
    LabelImage(LabelWidget *labelWidget, const cv::Mat& image);
    ~LabelImage();

    void addEdges(const cv::Mat& image);

    void buildKD();
    void searchNN(const QPointF& pos, EdgeItem*& pEdge, int& localIndex);
    void searchNN(const QPointF& pos, EdgeItem*& pEdge);

    void updateNNMask(EdgeItem* pEdge);

    QPointF item2image(const QPointF& pos);
    QPointF image2item(const QPointF& pos);

    EdgeItem* currEdge();
    void splitEdge();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QImage qimage;
    LabelWidget* parent;
    std::set<EdgeItem*> pEdges;
    EdgeItem* pCurrEdge;
    QPointF mousePos;

    cv::flann::Index* kdtree;
    std::vector<cv::Point2f> edgePoints;
    std::map<int, EdgeItem*> ind2edge;
    std::map<EdgeItem*, int> edge2ind;
    std::map<int, int> global2local;
    std::map<int, bool> pointMask;
    double radiusNN;
};

#endif // LABELIMAGE_H
