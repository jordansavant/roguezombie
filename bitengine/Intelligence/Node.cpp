#include "Node.hpp"
#include "SFML/System.hpp"

bit::Node::Node()
    : aStarID(0), fCost(0), gCost(0), hCost(0), x(0), y(0), aStarParent(NULL), parentContainer(NULL)
{
}

bit::Node::Node(int x, int y, NodeContainer* parentContainer)
    : aStarID(0), fCost(0), gCost(0), hCost(0), x(x), y(y), aStarParent(NULL), parentContainer(parentContainer)
{
}

bool bit::Node::equals(Node* compareNode)
{
    return (x == compareNode->x && y == compareNode->y);
}

void bit::Node::cleanAstar(unsigned int aStarID)
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

void bit::Node::resetAstar()
{
    gCost = 0;
    hCost = 0;
    fCost = 0;
    opened = false;
    closed = false;
    aStarParent = NULL;
}