#pragma once
#ifndef BIT_ASTARNODE_H
#define BIT_ASTARNODE_H

namespace bit
{
    class AstarNode
    {
    public:

        AstarNode();

        AstarNode(int x, int y);

        unsigned int aStarID;
        bool aStarClosed;
        bool aStarOpened;
        int aStarFCost;
        int aStarGCost;
        int aStarHCost;
        int aStarX;
        int aStarY;
        AstarNode* aStarParent;

        bool equals(AstarNode* compareNode);

        void cleanAstar(unsigned int aStarID);

    private:
        void resetAstar();
    };
}

#endif