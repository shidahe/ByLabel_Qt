#include "mat_qimage.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <QObject>
#include <QMessageBox>

namespace
{

inline
QImage mat_to_qimage_ref_in(cv::Mat const &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows,
                  static_cast<int>(mat.step), format);
}

inline
cv::Mat qimage_to_mat_ref_in(QImage const &img, int format)
{
    return cv::Mat(img.height(), img.width(),
                   format, const_cast<uchar*>(img.bits()), img.bytesPerLine());
}

} //end of namespace

/**
 *@brief make Qimage and cv::Mat share the same buffer, the resource
 * of the cv::Mat must not deleted before the QImage finish
 * the jobs.
 *
 *@param mat : input mat
 *@param swap : true : swap BGR to RGB; false, do nothing
 */
QImage mat_to_qimage_ref(cv::Mat const &mat, bool swap)
{
    if(!mat.empty()){

        // QImage needs the data to be stored continuously in memory
        if(!mat.isContinuous()){
            QMessageBox messageBox;
            messageBox.critical(0, QObject::tr("Error"), QObject::tr("Image matrix not continuous!"));
            return {};
        }

        switch(mat.type()){

        case CV_8UC3 :{
            if(swap){
                return mat_to_qimage_ref_in(mat, QImage::Format_RGB888).rgbSwapped();
            }else{
                return mat_to_qimage_ref_in(mat, QImage::Format_RGB888);
            }
        }

        case CV_8U :{
            return mat_to_qimage_ref_in(mat, QImage::Format_Indexed8);
        }

        case CV_8UC4 :{
           return mat_to_qimage_ref_in(mat, QImage::Format_ARGB32);
        }
        default: {
            QMessageBox messageBox;
            messageBox.critical(0, QObject::tr("Error"), QObject::tr("Unknown image color format!"));
            break;
        }
        }
    } else {
        QMessageBox messageBox;
        messageBox.warning(0, QObject::tr("Warning"), QObject::tr("Empty image!"));
    }

    return {};
}

/**
 *@brief copy cv::Mat into QImage
 *
 *@param mat : input mat
 *@param swap : true : swap BGR to RGB; false, do nothing
 */
QImage mat_to_qimage_cpy(cv::Mat const &mat, bool swap)
{
    return mat_to_qimage_ref(const_cast<cv::Mat&>(mat), swap).copy();
}

/**
 *@brief transform QImage to cv::Mat by sharing the buffer
 *@param img : input image
 *@param swap : true : swap RGB to BGR; false, do nothing
 */
cv::Mat qimage_to_mat_ref(QImage const &img, bool swap)
{
    if(img.isNull()){
        QMessageBox messageBox;
        messageBox.warning(0, QObject::tr("Warning"), QObject::tr("Empty image!"));
        return cv::Mat();
    }

    switch (img.format()) {
    case QImage::Format_RGB888:{
        auto result = qimage_to_mat_ref_in(img, CV_8UC3);
        if(swap){
            cv::cvtColor(result, result, CV_RGB2BGR);
        }
        return result;
    }
    case QImage::Format_Indexed8:{
        return qimage_to_mat_ref_in(img, CV_8U);
    }
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:{
        return qimage_to_mat_ref_in(img, CV_8UC4);
    }
    default: {
        QMessageBox messageBox;
        messageBox.critical(0, QObject::tr("Error"), QObject::tr("Unknown image color format!"));
        break;
    }
    }

    return {};
}

/**
 *@brief transform QImage to cv::Mat by copy QImage to cv::Mat
 *@param img : input image
 *@param swap : true : swap RGB to BGR; false, do nothing
 */
cv::Mat qimage_to_mat_cpy(QImage const &img, bool swap)
{
    return qimage_to_mat_ref(const_cast<QImage&>(img), swap).clone();
}
