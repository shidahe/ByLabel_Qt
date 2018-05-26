#ifndef LABELWIDGET_H
#define LABELWIDGET_H

#include <QGraphicsView>
#include <opencv2/core/core.hpp>
#include "opencv2/flann/miniflann.hpp"

class LabelImage;
class EdgeItem;

class LabelWidget : public QGraphicsView
{
    Q_OBJECT
public:
    LabelWidget(QWidget *parent = 0);

    void reset();

    void showImage(const cv::Mat& image);
    void addEdges(const cv::Mat& image);

    void buildKD();
    EdgeItem* getNN(const QPointF& pos);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void scaleView(qreal scaleFactor);

private:
    LabelImage* pImage;
    std::vector<EdgeItem*> pEdges;

    cv::flann::Index* kdtree;
    std::vector<cv::Point2f> edgePoints;
    std::map<int, EdgeItem*> ind2edge;
    double radiusNN;
};

#endif // LABELWIDGET_H
