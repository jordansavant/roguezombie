#include "AstarNode.hpp"
#include "SFML/System.hpp"

bit::AstarNode::AstarNode()
    : aStarID(0), aStarFCost(0), aStarGCost(0), aStarHCost(0), aStarX(0), aStarY(0), aStarParent(NULL)
{
}

bit::AstarNode::AstarNode(int x, int y)
    : aStarID(0), aStarFCost(0), aStarGCost(0), aStarHCost(0), aStarX(x), aStarY(y), aStarParent(NULL)
{
}

bool bit::AstarNode::equals(AstarNode* compareNode)
{
    return (aStarX == compareNode->aStarX && aStarY == compareNode->aStarY);
}

void bit::AstarNode::cleanAstar(unsigned int aStarID)
{
    // This allows us to reset in constant
    // time each time astar is run by avoiding
    // iterating across lists in astar to reset
    // nodes.
    if(this->aStarID != aStarID)
    {
        this->aStarID = aStarID;
        resetAstar();
    }
}

void bit::AstarNode::resetAstar()
{
    aStarGCost = 0;
    aStarHCost = 0;
    aStarFCost = 0;
    aStarOpened = false;
    aStarClosed = false;
    aStarParent = NULL;
}