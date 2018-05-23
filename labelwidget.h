#ifndef LABELWIDGET_H
#define LABELWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <opencv2/core/core.hpp>

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

public slots:

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif

    void scaleView(qreal scaleFactor);

private:
    LabelImage* pImage;
    std::vector<EdgeItem *> pEdges;
};

#endif // LABELWIDGET_H
