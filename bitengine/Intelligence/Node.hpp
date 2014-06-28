#pragma once
#ifndef BIT_NODE_H
#define BIT_NODE_H

namespace bit
{
    class Node
    {
    public:

        Node();

        Node(int x, int y);

        unsigned int aStarID;
        bool aStarClosed;
        bool aStarOpened;
        int aStarFCost;
        int aStarGCost;
        int aStarHCost;
        int aStarX;
        int aStarY;
        Node* aStarParent;

        bool equals(Node* compareNode);

        void cleanAstar(unsigned int aStarID);

    private:
        void resetAstar();
    };
}

#endif