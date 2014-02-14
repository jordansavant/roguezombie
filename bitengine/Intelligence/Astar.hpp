#pragma once
#ifndef BIT_ASTAR_H
#define BIT_ASTAR_H

#include <functional>
#include <vector>
#include <list>
#include <complex>
#include "Node.hpp"

namespace bit
{
    class Astar
    {
    public:

        static unsigned int aStarID;

        template<class T>
        static std::vector<T*> pathfind(T* startNodeContainer, T* endNodeContainer, std::function<bool(T*)> isBlocked, std::function<std::vector<T*>(T*)> getNeighbors)
        {
            aStarID++;

            startNodeContainer->node->cleanAstar(aStarID);
            endNodeContainer->node->cleanAstar(aStarID);

            std::list<T*> OpenList;
            std::list<T*> ClosedList;
            std::vector<T*> path;

            // add start node to open list
            T* currentNodeContainer = startNodeContainer;

            ClosedList.push_back(currentNodeContainer);
            currentNodeContainer->node->closed = true;

            // This was gone and made a massive memory leak before, it is vital!
            OpenList.push_back(currentNodeContainer);
            currentNodeContainer->node->opened = true;

            // Perform the path search
            while (!currentNodeContainer->node->equals(endNodeContainer->node))
            {
                // Mechanism for comparing neighbors
                std::vector<T*> surrounding = getNeighbors(currentNodeContainer);

                // Loop to look for best candidate via A*
                for (int i = 0; i < surrounding.size(); i++)
                {
                    T* checkNodeContainer = surrounding[i];
                    checkNodeContainer->node->cleanAstar(aStarID);

                    int xdiff;
                    int ydiff;
                    int gcost;
                    int hcost;

                    // G cost for this node
                    if (checkNodeContainer->node->gCost == 0)
                    {
                        // G = Cost to move from current active node to this node
                        //     If this is a locked down grid, you can treat horizontal neighbors
                        //     as a straight 10 and diagonal neighbors as a 14
                        //     If this is node mapping is not a locked down grid, perhaps a web
                        //     or something else, then calculate the distance realistically.
                        xdiff = std::abs(checkNodeContainer->node->x - currentNodeContainer->node->x);
                        ydiff = std::abs(checkNodeContainer->node->y - currentNodeContainer->node->y);
                        gcost = 0;
                        if (ydiff > 0 && xdiff > 0)
                            // diagonal -- TODO: improve to either be dynamic distance or coded for grid, not both :(
                            gcost = (int)((float)(xdiff + ydiff) / 1.4); // 1.4 is rough diagonal length of a square
                        else
                            // straight
                            gcost = xdiff + ydiff; // one has to be zero so it is the length of one side

                        checkNodeContainer->node->gCost = gcost;
                    }

                    // H cost for this node
                    if (checkNodeContainer->node->hCost == 0)
                    {
                        // H = Cost to move from this node to the destination node.
                        //     Use manhattan distance (total x distance + total y distance)
                        //     Or use real distance squareRoot( x distance ^ 2, y distance ^ 2)
                        //     Or some other heuristic if you are brave
                        // TODO: Not Manahattan -- use this real distance or use manhattan!
                        xdiff = checkNodeContainer->node->x - endNodeContainer->node->x;
                        ydiff = checkNodeContainer->node->y - endNodeContainer->node->y;
                        hcost = (int)std::sqrt((std::pow(xdiff, 2) + std::pow(ydiff, 2)));
                        checkNodeContainer->node->hCost = hcost;
                    }

                    // F cost for this node
                    if (checkNodeContainer->node->fCost == 0)
                    {
                        // F = G + H
                        // F = Cost to move from current active node to this node
                        //     plus the cost for this mode to travel to the final node
                        //     (calculated by manhattan distance or real distance etc.)
                        checkNodeContainer->node->fCost = checkNodeContainer->node->gCost + checkNodeContainer->node->hCost;
                    }

                    // Skip nodes that are blocked or already closed
                    if (!isBlocked(checkNodeContainer) && !checkNodeContainer->node->closed)
                    {
                        // If the connected node is not in the open list, add it to the open list
                        // and set its parent to our current active node
                        if (!checkNodeContainer->node->opened)
                        {
                            checkNodeContainer->node->aStarParent = currentNodeContainer->node;
                            OpenList.push_back(checkNodeContainer);
                            checkNodeContainer->node->opened = true;
                        }

                        // If the connected node is already in the open list, check to see if
                        // the path from our current active node to this node is better
                        // than are current selection
                        else
                        {
                            // Check to see if its current G cost is less than the new G cost of the parent and the old G cost
                            gcost = checkNodeContainer->node->gCost + currentNodeContainer->node->gCost;
                            if (gcost < checkNodeContainer->node->gCost)
                            {
                                // If so, make the current node its new parent and recalculate the gcost, and fcost
                                checkNodeContainer->node->aStarParent = currentNodeContainer->node;
                                checkNodeContainer->node->gCost = gcost;
                                checkNodeContainer->node->fCost = checkNodeContainer->node->gCost + checkNodeContainer->node->hCost;
                            }
                        }
                    }
                } // End neighbor loop

                // At this point the open list has been updated to reflect new parents and costs

                // Find the node in the open list with the lowest F cost,
                // (the total cost from the current active node to the open node
                // and the guesstimated cost from the open node to the destination node)
                T* cheapOpenNode = NULL;
                typename std::list<T*>::iterator i;
                for (i = OpenList.begin(); i != OpenList.end(); ++i)
                {
                    // Compare the openList nodes for the lowest F Cost
                    if (cheapOpenNode == NULL) // initialize our cheapest open node
                    {
                        cheapOpenNode = (*i);
                        continue;
                    }

                    if ((*i)->node->fCost < cheapOpenNode->node->fCost)
                    {
                        // we found a cheaper open list node
                        cheapOpenNode = (*i);
                    }
                }

                // We have run out of options, no shortest path, circumvent and leave
                if (cheapOpenNode == NULL)
                {
                    return std::vector<T*>();
                }

                // Now we have the node from the open list that has the cheapest F cost
                // move it to the closed list and set it as the current node
                OpenList.remove(cheapOpenNode);
                cheapOpenNode->node->opened = false;

                ClosedList.push_back(cheapOpenNode);
                cheapOpenNode->node->closed = true;

                currentNodeContainer = cheapOpenNode;
            } // A* Complete

            // we have found the end node
            // Loop from the current/end node moving back through the parents until we reach the start node
            // add those to the list and we have our path
            T* workingNodeContainer = currentNodeContainer;
            while (true)
            {
                // If I have traversed back to the beginning of the linked path
                if (workingNodeContainer->node->aStarParent == NULL)
                {
                    // Push the final game object
                    path.push_back(workingNodeContainer);
                    break;
                }
                // If I have more traversal to do
                else
                {
                    // Push the current game object
                    path.push_back(workingNodeContainer);

                    // Update my working object to my next parent
                    workingNodeContainer = static_cast<T*>(workingNodeContainer->node->aStarParent->parentContainer);
                }
            }

            return path;
        }
    };
}

#endif
