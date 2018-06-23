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

ConnectPoint::ConnectPoint(LabelImage* image, EndPoint* point1, EndPoint* point2, QPointF pos)
{
    pImage = image;
    pPoint1 = point1;
    pPoint2 = point2;
    pos2 = pos;
    createPoint = point2 == NULL;
}

void ConnectPoint::perform()
{
    if (createPoint) {
        pPoint2 = new EndPoint(pImage, pos2);
        pImage->addStrayPoint(pPoint2);
    }
    pImage->addConnection(pPoint1, pPoint2);
}

void ConnectPoint::reverse()
{
    if (createPoint) {
        pImage->removeStrayPoint(pPoint2);
        if (pPoint2) delete pPoint2;
    }
    pImage->removeConnection(pPoint1, pPoint2);
}
