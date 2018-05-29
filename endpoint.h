#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QGraphicsItem>
#include "edgeitem.h"

class EndPoint : public QGraphicsItem
{
public:
    EndPoint(EdgeItem* edgeItem, LabelImage* labelImage, int pointIndex);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPointF oldPos;
    float radius;
    QColor color;
    float borderWidth;
    float padding;
    EdgeItem* parent;
    LabelImage* image;
    int index;
};

#endif // ENDPOINT_H
