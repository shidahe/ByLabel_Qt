#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsItem>
#include <QPoint>

#include "labelwidget.h"

class LabelImage;

class EdgeItem : public QGraphicsItem
{
public:
    EdgeItem(LabelWidget *labelWidget, LabelImage *labelImage, const std::list<cv::Point>& points);

    QPointF center() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    std::list<QPointF> qpoints;
    std::list<QPointF> spoints;
    QRectF bbx;
    QColor color;
    float borderWidth;
    LabelImage* image;
    LabelWidget* parent;
};

#endif // EDGEITEM_H
