#ifndef MAT_QIMAGE_H
#define MAT_QIMAGE_H

#include <QImage>
#include <opencv2/core/core.hpp>

QImage mat_to_qimage_cpy(cv::Mat const &mat, bool swap = true);

QImage mat_to_qimage_ref(cv::Mat const &mat, bool swap = true);

cv::Mat qimage_to_mat_cpy(QImage const &img, bool swap = true);

cv::Mat qimage_to_mat_ref(QImage const &img, bool swap = true);

#endif // MAT_QIMAGE_H
