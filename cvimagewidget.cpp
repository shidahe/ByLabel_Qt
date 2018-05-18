#include "cvimagewidget.h"
#include <opencv2/imgproc/imgproc.hpp>

void CVImageWidget::showImage(const cv::Mat& image) {
    _qimage = mat_to_qimage_ref(image);
    this->setFixedSize(image.cols, image.rows);
    repaint();
}

void CVImageWidget::paintEvent(QPaintEvent* /*event*/) {
    // Display the image
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), _qimage);
    painter.end();
}
