#include "labelimage.h"
#include "mat_qimage.h"

LabelImage::LabelImage(LabelWidget *labelWidget, const cv::Mat& image)
    : parent(labelWidget)
{
    qimage = mat_to_qimage_ref(image);
    setZValue(-1);
}

QRectF LabelImage::boundingRect() const
{
    return QRectF(-qimage.width()/2, -qimage.height()/2, qimage.width(), qimage.height());
}

void LabelImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option);

    QRectF target(-qimage.width()/2, -qimage.height()/2, qimage.width(), qimage.height());
    QRectF source(0.0, 0.0, qimage.width(), qimage.height());

    painter->drawImage(target, qimage, source);
}
