#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QGraphicsObject>
#include "edgeitem.h"

class EndPoint : public QGraphicsObject
{
    Q_OBJECT
public:
    EndPoint(EdgeItem* edgeItem, LabelImage* labelImage, unsigned int pointIndex);

    EndPoint(LabelImage* labelImage, QPointF position);

    unsigned int indexOnEdge() const;

    void moveTo(unsigned int newIndex);

    void blink();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    double radius;
    QColor color;
    QColor createModeColor;
    double borderWidth;
    double padding;
    EdgeItem* parent;
    LabelImage* image;
    unsigned int index;
    unsigned int oldIndex;
    QPointF oldPos;
};

#endif // ENDPOINT_H
