#pragma once
#ifndef BIT_NODE_H
#define BIT_NODE_H

namespace bit
{
    class NodeContainer;

    class Node
    {
    public:

        Node();

        Node(int x, int y, NodeContainer* parentContainer);

        int x;
        int y;
        unsigned int aStarID;
        bool closed;
        bool opened;
        int fCost;
        int gCost;
        int hCost;
        Node* aStarParent;
        NodeContainer* parentContainer;

        bool equals(Node* compareNode);

        void cleanAstar(unsigned int aStarID);

    private:
        void resetAstar();
    };
}

#endif