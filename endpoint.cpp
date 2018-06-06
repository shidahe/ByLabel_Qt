#include "endpoint.h"
#include "labelimage.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

EndPoint::EndPoint(EdgeItem* edgeItem, LabelImage* labelImage, unsigned int pointIndex)
    : parent(edgeItem), image(labelImage), index(pointIndex)
{
    QPointF imagePos = parent->points()[index] + QPointF(0.5,0.5);
    radius = 1;
    color = Qt::blue;
    borderWidth = 0.2;
    padding = borderWidth;

    setPos(image->image2item(imagePos));
    oldPos = pos();
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
//    setCacheMode(DeviceCoordinateCache);
    setZValue(2);
    setVisible(false);
}

unsigned int EndPoint::indexOnEdge() const
{
    return index;
}

QVariant EndPoint::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QPointF newPos = value.toPointF();
    if (change == ItemPositionChange && scene()) {
        QPointF inc, dec;
        double len_i, len_d, len_o;
        len_i = std::numeric_limits<double>::infinity();
        len_d = std::numeric_limits<double>::infinity();
        len_o = QLineF(newPos, oldPos).length();

        bool canIncrement = true;
        if (index+1 >= parent->points().size()) canIncrement = false;
        if (this == parent->head() && index+1 >= parent->tail()->indexOnEdge()) canIncrement = false;

        bool canDecrement = true;
        if (index <= 0) canDecrement = false;
        if (this == parent->tail() && index-1 <= parent->head()->indexOnEdge()) canDecrement = false;

        if (canIncrement) {
            inc = parent->points()[index+1] + QPointF(0.5, 0.5);
            inc = image->image2item(inc);
            len_i = QLineF(newPos, inc).length();
        }
        if (canDecrement) {
            dec = parent->points()[index-1] + QPointF(0.5, 0.5);
            dec = image->image2item(dec);
            len_d = QLineF(newPos, dec).length();
        }

        if (len_i <= len_d && len_i <= len_o){
            newPos = inc;
            index++;
        } else if (len_d <= len_i && len_d <= len_o){
            newPos = dec;
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
    parent->setShowSplit(false);
    update();
    QGraphicsItem::mousePressEvent(event);
}

void EndPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    parent->setShowSplit(true);
    EdgeItem* nnEdge = NULL;
    image->searchNN(event->pos(), nnEdge);
    if (parent != nnEdge)
        parent->hoverLeave();
    // instead of recompute the nn, update a mask to prevent hover on hidden pixels
    image->updateNNMask(parent);
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

