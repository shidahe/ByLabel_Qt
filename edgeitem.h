#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsObject>
#include <QPoint>
#include "labelwidget.h"

class LabelImage;
class EndPoint;

class EdgeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    EdgeItem(LabelImage *labelImage, const std::list<cv::Point>& points);
    EdgeItem(LabelImage *labelImage, const std::vector<QPointF>& points, int start, int end);
    ~EdgeItem();

    void init(LabelImage *labelImage, const std::vector<QPointF>& points, int start, int end);

    void removeFromScene();

    std::vector<QPointF> points() const;
    QPointF center() const;

    void createEndPoints();
    void createEndPoints(int headIndex, int tailIndex);
    EndPoint* head() const;
    EndPoint* tail() const;
    bool pointVisible(int pointIndex) const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void hoverEnter(const QPointF& pos, const int pointIndex);
    void hoverLeave();

    QPainterPath shapeSplitPointA() const;
    QPainterPath shapeSplitPointB() const;
    QPainterPath shapeSplitLine() const;
    double convertSplitIndex(const QPointF& pos, const int pointIndex);

    void setShowSplit(bool show);
    bool showingSplit() const;

    std::vector<EdgeItem*> split();
    void select();
    void unselect();
    bool isSelected() const;

    void blink();

public Q_SLOTS:
    void setBlinkParameters(int animationProgress); // percentage

private:
    bool selected;
    QColor colorDefault;
    QColor colorHover;
    QColor colorSelected;
    QColor colorBlink;

    EndPoint* pHead;
    EndPoint* pTail;
    double splitIndex;
    QColor colorSplitA;
    QColor colorSplitB;
    QColor colorSplitLine;
    double splitLineLength;
    double splitLineWidth;
    bool showSplit;

    std::vector<QPointF> qpoints;
    std::vector<QPointF> spoints;
    QRectF bbx;
    QColor color;
    double borderWidth;
    double initBorderWidth;
    double edgeWidth;
    double padding;
    LabelImage* image;
};

#endif // EDGEITEM_H
