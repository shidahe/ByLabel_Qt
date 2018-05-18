#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>

#include "mat_qimage.h"

class CVImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CVImageWidget(QWidget *parent = 0) : QWidget(parent) {}

    QSize sizeHint() const { return _qimage.size(); }
    QSize minimumSizeHint() const { return _qimage.size(); }

public slots:
    void showImage(const cv::Mat& image);

protected:
    void paintEvent(QPaintEvent* /*event*/);

    QImage _qimage;
};

#endif // CVIMAGEWIDGET_H
