#include "action.h"

Action::Action()
{

}

SplitEdge::SplitEdge(EdgeItem *pEdge)
{
    oldEdge = pEdge;
    newEdge1 = NULL;
    newEdge2 = NULL;
}

void SplitEdge::perform()
{

}

void SplitEdge::reverse()
{

}
