#ifndef LABELWIDGET_H
#define LABELWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <opencv2/core/core.hpp>

class LabelImage;

class LabelWidget : public QGraphicsView
{
    Q_OBJECT
public:
    LabelWidget(QWidget *parent = 0);

    void showImage(const cv::Mat& image);

public slots:

protected:

#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif

    void scaleView(qreal scaleFactor);

private:
    LabelImage* pImage;
};

#endif // LABELWIDGET_H
