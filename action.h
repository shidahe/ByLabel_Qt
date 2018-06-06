#ifndef ACTION_H
#define ACTION_H

#include "edgeitem.h"
#include "labelimage.h"

class Action
{
public:
    virtual void perform();
    virtual void reverse();
};


class SplitEdge : Action
{
public:
    SplitEdge(EdgeItem* pEdge);

    void perform() override;
    void reverse() override;

private:
    EdgeItem* oldEdge;
    EdgeItem* newEdge1;
    EdgeItem* newEdge2;
};

class SelectEdge: Action
{
public:
    SelectEdge(EdgeItem* pEdge);

    void perform() override;
    void reverse() override;

private:
    EdgeItem* edge;
};


#endif // ACTION_H
