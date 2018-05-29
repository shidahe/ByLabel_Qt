#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QGraphicsItem>
#include <QImage>
#include "labelwidget.h"
#include <opencv2/flann/miniflann.hpp>

class LabelImage : public QGraphicsItem
{
public:
    LabelImage(LabelWidget *labelWidget, const cv::Mat& image);
    ~LabelImage();

    void addEdges(const cv::Mat& image);

    void buildKD();
    EdgeItem* getNN(const QPointF& pos);

    QPointF item2image(const QPointF& pos);
    QPointF image2item(const QPointF& pos);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QImage qimage;
    LabelWidget* parent;
    std::vector<EdgeItem*> pEdges;

    cv::flann::Index* kdtree;
    std::vector<cv::Point2f> edgePoints;
    std::map<int, EdgeItem*> ind2edge;
    double radiusNN;
};

#endif // LABELIMAGE_H
