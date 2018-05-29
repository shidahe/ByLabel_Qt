#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsItem>
#include <QPoint>
#include "labelwidget.h"

class LabelImage;
class EndPoint;

class EdgeItem : public QGraphicsItem
{
public:
    EdgeItem(LabelImage *labelImage, const std::list<cv::Point>& points);

    std::vector<QPointF> points() const;
    QPointF center() const;

    void createEndPoints();
    EndPoint* head() const;
    EndPoint* tail() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverEnter();
    void hoverLeave();

private:
    EndPoint* pHead;
    EndPoint* pTail;

    std::vector<QPointF> qpoints;
    std::vector<QPointF> spoints;
    QRectF bbx;
    QColor color;
    float borderWidth;
    float edgeWidth;
    float padding;
    LabelImage* image;
};

#endif // EDGEITEM_H
