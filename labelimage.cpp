#include "labelimage.h"
#include "mat_qimage.h"
#include "edgeitem.h"
#include <QGraphicsSceneHoverEvent>

LabelImage::LabelImage(LabelWidget *labelWidget, const cv::Mat& image)
    : parent(labelWidget)
{
    qimage = mat_to_qimage_ref(image);
    setZValue(-1);
    setAcceptHoverEvents(true);
}

QRectF LabelImage::boundingRect() const
{
    return QRectF(-qimage.width()/2.0, -qimage.height()/2.0, qimage.width(), qimage.height());
}

void LabelImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QRectF target(-qimage.width()/2.0, -qimage.height()/2.0, qimage.width(), qimage.height());
    QRectF source(0.0, 0.0, qimage.width(), qimage.height());

    painter->drawImage(target, qimage, source);
}

void LabelImage::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = event->lastPos() - boundingRect().topLeft();
    EdgeItem* prev = parent->getNN(lastPos);
    if(prev) prev->hoverLeave();
    QPointF pos = event->pos() - boundingRect().topLeft();
    EdgeItem* curr = parent->getNN(pos);
    if(curr) curr->hoverEnter();
}

void LabelImage::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF pos = event->pos() - boundingRect().topLeft();
    EdgeItem* curr = parent->getNN(pos);
    if(curr) curr->hoverEnter();
}

void LabelImage::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF lastPos = event->lastPos() - boundingRect().topLeft();
    EdgeItem* prev = parent->getNN(lastPos);
    if(prev) prev->hoverLeave();
}


