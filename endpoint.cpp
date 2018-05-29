#include "endpoint.h"
#include "labelimage.h"
#include <QDebug>

EndPoint::EndPoint(EdgeItem* edgeItem, LabelImage* labelImage, int pointIndex)
    : parent(edgeItem), image(labelImage), index(pointIndex)
{
    QPointF imagePos = parent->points()[index] + QPointF(0.5,0.5);
    radius = 1;
    color = Qt::darkRed;
    borderWidth = 0.2;
    padding = borderWidth;

    setPos(image->image2item(imagePos));
    oldPos = pos();
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setZValue(2);
    setVisible(false);
}

QVariant EndPoint::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QPointF newPos = value.toPointF();
    if (change == ItemPositionChange && scene()) {
        QPointF forward, backward;
        double len_f, len_b, len_o;
        len_f = std::numeric_limits<double>::infinity();
        len_b = std::numeric_limits<double>::infinity();
        len_o = QLineF(newPos, oldPos).length();

        if (index < int(parent->points().size())-1) {
            forward = parent->points()[index+1] + QPointF(0.5, 0.5);
            forward = image->image2item(forward);
            len_f = QLineF(newPos, forward).length();
        }
        if (index > 0) {
            backward = parent->points()[index-1] + QPointF(0.5, 0.5);
            backward = image->image2item(backward);
            len_b = QLineF(newPos, backward).length();
        }

//        qDebug() << oldPos << newPos;
//        qDebug() << len_f << len_b << len_o;
        if (len_f <= len_b && len_f <= len_o){
            newPos = forward;
            index++;
        } else if (len_b <= len_f && len_b <= len_o){
            newPos = backward;
            index--;
        } else {
            newPos = oldPos;
        }
        oldPos = newPos;
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}

void EndPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    oldPos = pos();
    update();
    QGraphicsItem::mousePressEvent(event);
}

void EndPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

QRectF EndPoint::boundingRect() const
{
    return QRectF(-radius-padding, -radius-padding,
                  (radius+padding)*2, (radius+padding)*2);
}

QPainterPath EndPoint::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0,0), radius, radius);
    return path;
}

void EndPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(color, borderWidth));
    painter->setBrush(QBrush(color));
    painter->drawPath(shape());
}
