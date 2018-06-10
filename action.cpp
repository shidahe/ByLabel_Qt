#include "action.h"
#include "edgeitem.h"
#include "labelimage.h"
#include "endpoint.h"
#include <QDebug>

EndPointMove::EndPointMove(EndPoint *pPoint, unsigned int oldInd, unsigned int newInd)
{
    endPoint = pPoint;
    oldIndex = oldInd;
    newIndex = newInd;
}

void EndPointMove::perform()
{
    endPoint->moveTo(newIndex);
    endPoint->blink();
}

void EndPointMove::reverse()
{
    endPoint->moveTo(oldIndex);
    endPoint->blink();
}

SplitEdge::SplitEdge(LabelImage* pImage, EdgeItem *pEdge)
{
    image = pImage;
    oldEdge = pEdge;
    newEdge1 = NULL;
    newEdge2 = NULL;
}

void SplitEdge::perform()
{    
    if (!newEdge1 || !newEdge2) {
        std::vector<EdgeItem*> newEdges = oldEdge->split();
        if (newEdges.size() != 2) return;
        newEdge1 = newEdges[0];
        newEdge2 = newEdges[1];
    }
    image->performSplitEdge(oldEdge, newEdge1, newEdge2);
    newEdge1->blink();
    newEdge2->blink();
}

void SplitEdge::reverse()
{
    if (!newEdge1 || !newEdge2) return;
    image->reverseSplitEdge(oldEdge, newEdge1, newEdge2);
    oldEdge->blink();
}

SelectEdge::SelectEdge(EdgeItem* pEdge)
{
    edge = pEdge;
}

void SelectEdge::perform()
{
    edge->select();
}

void SelectEdge::reverse()
{
    edge->unselect();
    edge->blink();
}



