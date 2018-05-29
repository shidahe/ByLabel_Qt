#include "edgeitem.h"
#include "labelimage.h"
#include "endpoint.h"
#include <QtDebug>

EdgeItem::EdgeItem(LabelImage* labelImage, const std::list<cv::Point>& points)
    : image(labelImage)
{
    QPointF tl(points.front().x, points.front().y);
    QPointF br(points.front().x, points.front().y);
    QPointF prev;

    for (auto point : points) {
        QPointF curr(point.x, point.y);
        // ignore duplicate pixels at same location
        if (!prev.isNull() && prev == curr) continue;
        if (curr.x() < tl.x()) tl.setX(curr.x());
        if (curr.x() > br.x()) br.setX(curr.x());
        if (curr.y() < tl.y()) tl.setY(curr.y());
        if (curr.y() > br.y()) br.setY(curr.y());
        qpoints.push_back(curr);
        prev = curr;
    }
    bbx.setTopLeft(tl);
    bbx.setBottomRight(br);

    // spoints: points in local coordinate (origin at bounding rectangle center)
    for (auto point : qpoints)
        spoints.push_back(point - (br + tl)/2);

    color = Qt::green;
    borderWidth = 0.1;
    edgeWidth = 1.2;
    // padding to the bouding rectangle, need to cover the border of edge pixels
    padding = (edgeWidth-1)/2 + borderWidth;

    pHead = NULL;
    pTail = NULL;

    setZValue(0);
}

void EdgeItem::createEndPoints()
{
    if (pHead) delete pHead;
    pHead = new EndPoint(this, image, 0);
    scene()->addItem(pHead);

    if (pTail) delete pTail;
    pTail = new EndPoint(this, image, qpoints.size()-1);
    scene()->addItem(pTail);
}

EndPoint* EdgeItem::head() const
{
    return pHead;
}

EndPoint* EdgeItem::tail() const
{
    return pTail;
}

std::vector<QPointF> EdgeItem::points() const
{
    return qpoints;
}

QPointF EdgeItem::center() const
{
    // center of bounding rectangle in scene coordinate
    return QPointF(image->boundingRect().left() + (bbx.left()+bbx.right())/2,
                   image->boundingRect().top() + (bbx.top()+bbx.bottom())/2);
}

QRectF EdgeItem::boundingRect() const
{
    // bounding rectangle in local coordinate
    return QRectF(-bbx.width()/2-padding, -bbx.height()/2-padding,
                  bbx.width()+1+padding*2, bbx.height()+1+padding*2);
}

QPainterPath EdgeItem::shape() const
{
    // rectangles as pixels
    float dist = (edgeWidth-1)/2;
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    for (auto point : spoints){
        path.addRect(point.x()-dist, point.y()-dist, dist*2+1, dist*2+1);
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

void EdgeItem::hoverEnter()
{
    color = Qt::red;
    setZValue(1);
    if(pHead) pHead->setVisible(true);
    if(pTail) pTail->setVisible(true);
    update(boundingRect());
}

void EdgeItem::hoverLeave()
{
    color = Qt::green;
    setZValue(0);
    if(pHead) pHead->setVisible(false);
    if(pTail) pTail->setVisible(false);
    update(boundingRect());
}

