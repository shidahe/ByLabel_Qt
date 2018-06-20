#ifndef ACTION_H
#define ACTION_H

class EdgeItem;
class EndPoint;
class LabelImage;

class Action
{
public:
    virtual ~Action() = default;
    virtual void perform() = 0;
    virtual void reverse() = 0;
};

class EndPointMove : public Action
{
public:
    EndPointMove(EndPoint* pPoint, unsigned int oldInd, unsigned int newInd);

    void perform() override;
    void reverse() override;

private:
    EndPoint* endPoint;
    unsigned int oldIndex;
    unsigned int newIndex;
};

class SplitEdge : public Action
{
public:
    SplitEdge(LabelImage* pImage, EdgeItem* pEdge);

    void perform() override;
    void reverse() override;

private:
    LabelImage* image;
    EdgeItem* oldEdge;
    EdgeItem* newEdge1;
    EdgeItem* newEdge2;
};

class SelectEdge: public Action
{
public:
    SelectEdge(EdgeItem* pEdge);

    void perform() override;
    void reverse() override;

private:
    EdgeItem* edge;
};

class ConnectPoint: public Action
{
public:
    ConnectPoint(LabelImage* pImage, EndPoint* pPoint1, EndPoint* pPoint2);

    void perform() override;
    void reverse() override;

private:
    LabelImage* pImage;
    EndPoint* pPoint1;
    EndPoint* pPoint2;
};


#endif // ACTION_H
