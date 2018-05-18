#include "labelwidget.h"
#include "labelimage.h"
#include <QKeyEvent>

LabelWidget::LabelWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::gray);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(1.0), qreal(1.0));
    setMinimumSize(400, 400);
    pImage = NULL;
}

void LabelWidget::showImage(const cv::Mat& image)
{
    if (pImage) {
        scene()->removeItem(pImage);
        delete pImage;
    }
    pImage = new LabelImage(this, image);
    scene()->addItem(pImage);

    pImage->setPos(0,0);

    repaint();
}

#if QT_CONFIG(wheelevent)
void LabelWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void LabelWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

//void LabelWidget::drawBackground(QPainter *painter, const QRectF &rect)
//{
//    Q_UNUSED(rect);
//
//    painter->drawImage(rect.left(), rect.top(), _qimage);
//    painter->end();
//}

//void LabelWidget::paintEvent(QPaintEvent* /*event*/) {
//    // Display the image
//    QPainter painter(this);
//    painter.drawImage(QPoint(0,0), _qimage);
//    painter.end();
//}

