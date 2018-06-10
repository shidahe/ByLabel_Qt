#include "labelwidget.h"
#include "labelimage.h"
#include <QKeyEvent>
#include <QtDebug>

LabelWidget::LabelWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(Qt::gray);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
    pImage = NULL;
    setFocusPolicy(Qt::StrongFocus);
}

LabelWidget::~LabelWidget()
{
    reset();
}

void LabelWidget::reset()
{
    if (pImage) {
        scene()->removeItem(pImage);
        delete pImage;
        pImage = NULL;
    }

    resetMatrix();
}

void LabelWidget::showImage(const cv::Mat& image)
{
    reset();

    pImage = new LabelImage(this, image);
    scene()->addItem(pImage);
    pImage->setPos(0,0);
    pImage->addEdges(image);

    repaint();
    setFocus();
}

void LabelWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        setInteractive(false);
        setDragMode(ScrollHandDrag);
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    }
    else QGraphicsView::mousePressEvent(event);
}

void LabelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fake);
        setDragMode(NoDrag);
        setInteractive(true);
    }
    else QGraphicsView::mouseReleaseEvent(event);
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

void LabelWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        pImage->splitEdge();
        break;
    case Qt::Key_R:
        pImage->reverseAction();
        break;
    case Qt::Key_T:
        pImage->redoAction();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

