#include "edgeitem.h"
#include "labelimage.h"

#include <QtDebug>

EdgeItem::EdgeItem(LabelWidget *labelWidget, LabelImage* labelImage, const std::list<cv::Point>& points)
    : image(labelImage), parent(labelWidget)
{
    QPointF tl(points.front().x, points.front().y);
    QPointF br(points.front().x, points.front().y);
    QPointF prev;

    for (auto point : points) {
        QPointF curr(point.x, point.y);
        if (!prev.isNull() && prev == curr) continue;
        if (curr.x() < tl.x()) tl.setX(curr.x());
        if (curr.x() > br.x()) br.setX(curr.x());
        if (curr.y() < tl.y()) tl.setY(curr.y());
        if (curr.y() > br.y()) br.setY(curr.y());
        qpoints.push_back(curr);
        prev = curr;
//        qDebug() << curr.x() << curr.y();
    }
    bbx.setTopLeft(tl);
    bbx.setBottomRight(br);

    for (auto point : qpoints)
        spoints.push_back(point - (br + tl)/2);

    color = Qt::green;
    borderWidth = 0.0;

    setZValue(0);
//    setFlag(ItemIsMovable);
    setAcceptHoverEvents(true);
}

QPointF EdgeItem::center() const
{
    return QPointF(image->boundingRect().left() + (bbx.left() + bbx.right())/2,
                   image->boundingRect().top() + (bbx.top() + bbx.bottom())/2);
}

QRectF EdgeItem::boundingRect() const
{
    int padding = 2;
    return QRectF(-(bbx.width()+padding)/2 , -(bbx.height()+padding)/2, bbx.width()+padding, bbx.height()+padding);
}

QPainterPath EdgeItem::shape() const
{
    QPainterPath path;
    for (auto point : spoints){
        path.addRect(point.x(), point.y(), 1, 1);
    }
    return path;
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(color, borderWidth));
    painter->setBrush(QBrush(color));
    painter->drawPath(shape());
}

void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    color = Qt::red;
    setZValue(1);
    update(boundingRect());
}

void EdgeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    color = Qt::green;
    setZValue(0);
    update(boundingRect());
}

