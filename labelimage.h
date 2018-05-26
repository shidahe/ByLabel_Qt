#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QGraphicsItem>
#include <QImage>

#include "labelwidget.h"

class LabelImage : public QGraphicsItem
{
public:
    LabelImage(LabelWidget *labelWidget, const cv::Mat& image);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QImage qimage;
    LabelWidget* parent;
};

#endif // LABELIMAGE_H
