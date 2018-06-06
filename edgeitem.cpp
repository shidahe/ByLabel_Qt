#include "edgeitem.h"
#include "labelimage.h"
#include "endpoint.h"
#include <QtDebug>

EdgeItem::EdgeItem(LabelImage* labelImage, const std::list<cv::Point>& points)
{
    std::vector<QPointF> temp;
    for (auto point : points) {
        QPointF curr(point.x, point.y);
        temp.push_back(curr);
    }
    init(labelImage, temp, 0, temp.size()-1);
}

EdgeItem::EdgeItem(LabelImage *labelImage, const std::vector<QPointF>& points, int start, int end)
{
    init(labelImage, points, start, end);
}

EdgeItem::~EdgeItem()
{
    removeFromScene();
}

void EdgeItem::removeFromScene()
{
    if (pHead){
        if (scene())
            scene()->removeItem(pHead);
        delete pHead;
    }
    if (pTail){
        if (scene())
            scene()->removeItem(pTail);
        delete pTail;
    }

    if (scene())
        scene()->removeItem(this);
}

void EdgeItem::init(LabelImage *labelImage, const std::vector<QPointF>& points, int start, int end)
{
    image = labelImage;

    QPointF tl(points[start].x(), points[start].y());
    QPointF br(points[start].x(), points[start].y());
    QPointF prev;

    for (int i = start; i <= end; i++) {
        QPointF curr(points[i]);
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
    splitLineLength = 6;
    splitLineWidth = 0.3;
    // padding to the bouding rectangle, need to cover border and split line
    padding = std::max((edgeWidth-1)/2 + borderWidth, splitLineLength/2 - 0.5);

    splitIndex = -0.25;
    colorSplitA = Qt::cyan;
    colorSplitB = Qt::cyan;
    colorSplitLine = Qt::yellow;
    showSplit = false;

    pHead = NULL;
    pTail = NULL;

    setZValue(0);
//    setCacheMode(DeviceCoordinateCache);
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

void EdgeItem::createEndPoints(int headIndex, int tailIndex)
{
    if (pHead) delete pHead;
    pHead = new EndPoint(this, image, headIndex);
    scene()->addItem(pHead);

    if (pTail) delete pTail;
    pTail = new EndPoint(this, image, tailIndex);
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

bool EdgeItem::pointVisible(int pointIndex) const
{
    bool afterHead = pHead ? pointIndex >= (int)(pHead->indexOnEdge()) : pointIndex >= 0;
    bool beforeTail = pTail ? pointIndex <= (int)(pTail->indexOnEdge()) : pointIndex < (int)points().size();
    return afterHead && beforeTail;
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
    for (unsigned int i = 0; i < points().size(); i++)
        if (pointVisible(i))
            path.addRect(spoints[i].x()-dist, spoints[i].y()-dist, dist*2+1, dist*2+1);

    return path;
}

QPainterPath EdgeItem::shapeSplitPointA() const
{
    float dist = (edgeWidth-1)/2;
    QPainterPath path;
    int r = round(splitIndex);
    int i = splitIndex > r ? r : r-1;
    if (pointVisible(i))
        path.addRect(spoints[i].x()-dist, spoints[i].y()-dist, dist*2+1, dist*2+1);
    return path;
}

QPainterPath EdgeItem::shapeSplitPointB() const
{
    float dist = (edgeWidth-1)/2;
    QPainterPath path;
    int r = round(splitIndex);
    int i = splitIndex > r ? r+1 : r;
    if (pointVisible(i))
        path.addRect(spoints[i].x()-dist, spoints[i].y()-dist, dist*2+1, dist*2+1);
    return path;
}

QPainterPath EdgeItem::shapeSplitLine() const
{
    // draw a line between two pixels
    float half = splitLineLength/2;
    QPainterPath path;
    int r = round(splitIndex);
    int a = splitIndex > r ? r : r-1;
    int b = splitIndex > r ? r+1 : r;

    if (pointVisible(a) && pointVisible(b)){
        QPointF pointA = spoints[a] + QPointF(0.5, 0.5);
        QPointF pointB = spoints[b] + QPointF(0.5, 0.5);
        QPointF mid = (pointA + pointB) / 2;
        QPointF d = (pointB - pointA) / QLineF(pointA, pointB).length();
        QLineF n = QLineF(QPointF(0,0), d).normalVector();
        path.moveTo(mid - n.p2()*half);
        path.lineTo(mid + n.p2()*half);
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

    if (showSplit) {
        painter->setPen(QPen(colorSplitA, borderWidth));
        painter->setBrush(QBrush(colorSplitA));
        painter->drawPath(shapeSplitPointA());
        painter->setPen(QPen(colorSplitB, borderWidth));
        painter->setBrush(QBrush(colorSplitB));
        painter->drawPath(shapeSplitPointB());
        painter->setPen(QPen(colorSplitLine, splitLineWidth, Qt::DashLine));
        painter->setBrush(QBrush(colorSplitLine));
        painter->drawPath(shapeSplitLine());
    }
}

double EdgeItem::convertSplitIndex(const QPointF& pos, const int pointIndex)
{
    QPointF forward, backward;
    double len_f, len_b;
    len_f = std::numeric_limits<double>::infinity();
    len_b = std::numeric_limits<double>::infinity();

    if (pointIndex+1 < (int)points().size()) {
        forward = points()[pointIndex+1] + QPointF(0.5, 0.5);
        len_f = QLineF(pos, forward).length();
    }
    if (pointIndex > 0) {
        backward = points()[pointIndex-1] + QPointF(0.5, 0.5);
        len_b = QLineF(pos, backward).length();
    }

    if (len_b <= len_f) {
        return pointIndex - 0.25;
    } else {
        return pointIndex + 0.25;
    }
}

void EdgeItem::hoverEnter(const QPointF& pos, const int pointIndex)
{
    showSplit = true;
    splitIndex = convertSplitIndex(pos, pointIndex);
    color = Qt::red;
    setZValue(1);
    if(pHead) pHead->setVisible(true);
    if(pTail) pTail->setVisible(true);
    update(boundingRect());
}

void EdgeItem::hoverLeave()
{
    showSplit = false;
    color = Qt::green;
    setZValue(0);
    if(pHead) pHead->setVisible(false);
    if(pTail) pTail->setVisible(false);
    update(boundingRect());
}

void EdgeItem::setShowSplit(bool show)
{
    showSplit = show;
}

bool EdgeItem::showingSplit() const
{
    return showSplit;
}

std::vector<EdgeItem*> EdgeItem::split()
{
    std::vector<EdgeItem*> edges;

    int r = round(splitIndex);
    int a = splitIndex > r ? r : r-1;
    int b = splitIndex > r ? r+1 : r;
    if (!pointVisible(a) || !pointVisible(b)) return edges;

    EdgeItem* edge1 = new EdgeItem(image, qpoints, 0, a);
    EdgeItem* edge2 = new EdgeItem(image, qpoints, b, qpoints.size()-1) ;

    edges.push_back(edge1);
    edges.push_back(edge2);

    return edges;
}

