#include "endpoint.h"
#include "labelimage.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "action.h"

EndPoint::EndPoint(EdgeItem* edgeItem, LabelImage* labelImage, unsigned int pointIndex)
    : parent(edgeItem), image(labelImage), index(pointIndex)
{
    QPointF imagePos = parent->points()[index] + QPointF(0.5,0.5);
    radius = 1;
    color = Qt::blue;
    createModeColor = Qt::yellow;
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

EndPoint::EndPoint(LabelImage* labelImage, QPointF position)
    : parent(NULL), image(labelImage), index(0)
{
    radius = 1;
    color = Qt::blue;
    createModeColor = Qt::yellow;
    borderWidth = 0.2;
    padding = borderWidth;

    setPos(position);
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
    if (change == ItemPositionChange && scene() && parent) {
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
    return QGraphicsObject::itemChange(change, value);
}

void EndPoint::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    oldPos = pos();
    if (parent) {
        oldIndex = index;
        parent->setShowSplit(false);
    }
    update(boundingRect());
    QGraphicsObject::mousePressEvent(event);
}

void EndPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (parent) {
        EdgeItem* nnEdge = NULL;
        // instead of recompute the nn, update a mask to prevent hover on hidden pixels
        image->updateNNMask(parent);
        image->searchNN(event->pos(), nnEdge);
        if (parent != nnEdge)
            parent->hoverLeave();
        // add action to queue
        if (oldIndex != index)
            image->addAction(new EndPointMove(this, oldIndex, index));
        if (!parent->isSelected())
            parent->setShowSplit(true);
    } else {
        //TODO: mouse handling for connecting points
        if (image->inCreateMode() && ! image->getConnectPoint()) {
            image->updateConnectPoint(this);
        } else if (image->inCreateMode() && image->getConnectPoint()) {
            Action* act = new ConnectPoint(image, image->getConnectPoint(), this, event->pos());
            act->perform();
            image->addAction(act);
            image->updateConnectPoint(NULL);
        }

    }
    update(boundingRect());
    QGraphicsObject::mouseReleaseEvent(event);
}

void EndPoint::moveTo(unsigned int newIndex)
{
    bool canMove = true;
    if (newIndex >= parent->points().size()) canMove = false;
    if (this == parent->head() && newIndex >= parent->tail()->indexOnEdge()) canMove = false;
    if (this == parent->tail() && newIndex <= parent->head()->indexOnEdge()) canMove = false;

    if (canMove) {
        index = newIndex;
        QPointF newPos = parent->points()[index] + QPointF(0.5, 0.5);
        newPos = image->image2item(newPos);
        oldPos = newPos;
        setPos(newPos); // will trriger itemChange
    }

    image->updateNNMask(parent);
    update(boundingRect());
    parent->update(parent->boundingRect());
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

    if (!image->inCreateMode()) {
        painter->setPen(QPen(color, borderWidth));
        painter->setBrush(QBrush(color));
    } else {
        painter->setPen(QPen(createModeColor, borderWidth));
        painter->setBrush(QBrush(createModeColor));
    }
    painter->drawPath(shape());
}

void EndPoint::blink()
{
    parent->blink();
}
