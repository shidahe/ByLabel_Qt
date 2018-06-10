#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include "labelwidget.h"
#include <set>
#include <opencv2/flann/miniflann.hpp>

class EndPoint;
class Action;

class LabelImage : public QGraphicsObject
{
    Q_OBJECT
public:
    LabelImage(LabelWidget *labelWidget, const cv::Mat& image);
    ~LabelImage();

    void addEdges(const cv::Mat& image);

    void buildKD();
    void searchNN(const QPointF& pos, EdgeItem*& pEdge, int& localIndex);
    void searchNN(const QPointF& pos, EdgeItem*& pEdge);

    void updateNNMask(EdgeItem* pEdge);

    QPointF item2image(const QPointF& pos);
    QPointF image2item(const QPointF& pos);

    EdgeItem* currEdge();
    void splitEdge();
    void performSplitEdge(EdgeItem* oldEdge, EdgeItem* newEdge1, EdgeItem* newEdge2);
    void reverseSplitEdge(EdgeItem* oldEdge, EdgeItem* newEdge1, EdgeItem* newEdge2);

    void addAction(Action* act);
    void reverseAction();
    void redoAction();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QImage qimage;
    LabelWidget* parent;
    std::set<EdgeItem*> pEdges;
    EdgeItem* pCurrEdge;

    // for hovering
    QPointF mousePos;
    cv::flann::Index* kdtree;
    std::vector<cv::Point2f> edgePoints;
    std::map<int, EdgeItem*> ind2edge;
    std::map<EdgeItem*, int> edge2ind;
    std::map<int, int> global2local;
    std::map<int, bool> pointMask;
    double radiusNN;

    // action queue
    unsigned int maxActionListSize;
    std::list<Action*> actionList;
    std::list<Action*> redoList;
};

#endif // LABELIMAGE_H
