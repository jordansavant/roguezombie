#pragma once
#ifndef BIT_QUADTREE_H
#define BIT_QUADTREE_H

#include <vector>
#include <iostream>
#include <sstream>

namespace bit
{
    template <class T>
    class QuadTree
    {
    public:
        QuadTree(float x, float y, float width, float height, int level = 0, int poolSize = 100)
            : level(level), x(x), y(y), width(width), height(height), pool(NULL)
        {
            maxObjects = 7;
            maxLevels = 10;

            if(level == 0)
            {
                // The root quadtree can maintain a pool
                pool = new std::vector<QuadTree*>();
                for(int i = 0; i < poolSize; i ++)
                {
                    pool->push_back(new QuadTree(0, 0, 0, 0, -1));
                }
            }
        }

        ~QuadTree()
        {
            if(level == 0)
            {
                for(int i = 0; i < pool->size(); i ++)
                {
                    if((*pool)[i])
                    {
                        delete (*pool)[i];
                    }
                }

                delete pool;
            }

            for(int i = 0; i < nodes.size(); i ++)
            {
                if(nodes[i])
                {
                    delete nodes[i];
                }
            }
        }

        int maxObjects;
        int maxLevels;
        int level;
        float x, y;
        float width, height;
        std::vector<T*> objects;
        std::vector<QuadTree*>* pool;
        std::vector<QuadTree*> nodes;

        void clear()
        {
            objects.clear();
            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i])
                {
                    nodes[i]->clear();
                }

                returnToPool(nodes[i]);
            }
            nodes.clear();
        }

        void split()
        {
            float subWidth =  width / 2;
            float subHeight = height / 2;
            float subX = x;
            float subY = y;

            nodes.push_back(fetchFromPool(subX, subY, subWidth, subHeight, level + 1));
            nodes.push_back(fetchFromPool(subX + subWidth, subY, subWidth, subHeight, level + 1));
            nodes.push_back(fetchFromPool(subX, subY + subHeight, subWidth, subHeight, level + 1));
            nodes.push_back(fetchFromPool(subX + subWidth, subY + subHeight, subWidth, subHeight, level + 1));
        }

        QuadTree* fetchFromPool(float _x, float _y, float _width, float _height, int _level)
        {
            QuadTree* fromPool;

            if(pool->size() > 0)
            {
                fromPool = pool->back();
                pool->pop_back();
            }
            else
            {
                fromPool = new QuadTree(0, 0, 0, 0, -1);
            }

            fromPool->level = _level;
            fromPool->x = _x;
            fromPool->y = _y;
            fromPool->width = _width;
            fromPool->height = _height;
            fromPool->pool = pool;
            fromPool->maxObjects = maxObjects;
            fromPool->maxLevels = maxLevels;

            return fromPool;
        }

        void returnToPool(QuadTree* quadTree)
        {
            quadTree->level = -1;
            quadTree->x = 0;
            quadTree->y = 0;
            quadTree->width = 0;
            quadTree->height = 0;
            quadTree->pool = NULL;
            pool->push_back(quadTree);
        }

        int getIndex(float _x, float _y, float _width, float _height)
        {
            float verticalMidpoint = x + (width / 2);
            float horizontalMidpoint = y + (height / 2);

            bool withinTop = _y >= y && _y + _height <= horizontalMidpoint;
            bool withinBottom = _y >= horizontalMidpoint && _y <= y + height;
            bool withinLeft = _x >= x && _x + _width <= verticalMidpoint;
            bool withinRight = _x >= verticalMidpoint && _x <= x + width;

            // Within Top
            if(withinTop)
            {
                // Within Left
                if (withinLeft)
                {
                    return 0;
                }
                // Within Right
                else if(withinRight)
                {
                    return 1;
                }
            }
            // Within Bottom
            else if(withinBottom)
            {
                // Within Left
                if (withinLeft)
                {
                    return 2;
                }
                // Within Right
                else if(withinRight)
                {
                    return 3;
                }
            }

            // Object does not fit, must belong to parent
            return -1;
        }

        int getIndex(T* pRect)
        {
            return getIndex(pRect->quadtree_getX(), pRect->quadtree_getY(), pRect->quadtree_getWidth(), pRect->quadtree_getHeight());
        }

        void insert(T* pRect)
        {
            // Insert into child via recursion
            if (nodes.size() > 0)
            {
                int index = getIndex(pRect);

                if (index != -1)
                {
                    nodes[index]->insert(pRect);

                    return;
                }
            }

            // Insert into self
            objects.push_back(pRect);

            // Handle capacity overload
            if (objects.size() > maxObjects && level < maxLevels)
            {
                // If we have exceeded capacity for this quadtree then split into new ones
                if (nodes.size() == 0)
                {
                    split();
                }

                // Move objects into newly created quadtree children
                int i = 0;
                while (i < objects.size())
                {
                    int index = getIndex(objects.at(i));

                    if (index != -1)
                    {
                        T* ob = objects.at(i);
                        objects.erase(objects.begin() + i);
                        nodes[index]->insert(ob);
                    }
                    else
                    {
                        i++;
                    }
                }
            }
        }

        void getNeighbors(std::vector<T*> &fill, T* pRect)
        {
            // Search into children to retrieve
            int index = getIndex(pRect);
            if (index != -1 && nodes.size() > 0)
            {
                nodes[index]->getNeighbors(fill, pRect);
            }
            // I added this from code inspection, have no idea if this else condition will help but it seemed like
            // this quadtree should only fill in the neighbors if it did not have an index for the children
            else
            {
                for(int i = 0; i < objects.size(); i++)
                {
                    fill.push_back(objects[i]);
                }
            }
        }

        void getAllObjects(std::vector<T*> &fill)
        {
            // Get my local objects
            for(unsigned int i=0; i < objects.size(); i++)
            {
                fill.push_back(objects[i]);
            }

            // Get all child node objects recursively
            for(unsigned int i=0; i < nodes.size(); i++)
            {
                nodes[i]->getAllObjects(fill);
            }
        }

        void getAllObjectsWithin(std::vector<T*> &fill, float _x, float _y, float _width, float _height)
        {
            // If we have child split nodes, look in them
            if(nodes.size() > 0)
            {
                int childIndex = getIndex(_x, _y, _width, _height);

                // This request does not fit into one of my children so it spans multiple children, therefore collect all objects
                if(childIndex == -1)
                {
                    int tl_index = getIndex(_x, _y, 0, 0); 						//  0 ->  1
                    int tr_index = getIndex(_x + _width, _y, 0, 0); 			//  1 -> -1
                    int bl_index = getIndex(_x, _y + _height, 0, 0); 			//  0 ->  1
                    int br_index = getIndex(_x + _width, _y + _height, 0, 0); 	//  1 -> -1

                    bool inTL = tl_index == 0 || tr_index == 0 || bl_index == 0 || br_index == 0;
                    bool inTR = tl_index == 1 || tr_index == 1 || bl_index == 1 || br_index == 1;
                    bool inBL = tl_index == 2 || tr_index == 2 || bl_index == 2 || br_index == 2;
                    bool inBR = tl_index == 3 || tr_index == 3 || bl_index == 3 || br_index == 3;

                    for(unsigned int i = 0; i < 4; i++)
                    {
                        switch(i)
                        {
                            case 0:
                                if(inTL)
                                {
                                    nodes[i]->getAllObjectsWithin(fill, _x, _y, _width, _height);
                                }
                                break;
                            case 1:
                                if(inTR)
                                {
                                    nodes[i]->getAllObjectsWithin(fill, _x, _y, _width, _height);
                                }
                                break;
                            case 2:
                                if(inBL)
                                {
                                    nodes[i]->getAllObjectsWithin(fill, _x, _y, _width, _height);
                                }
                                break;
                            case 3:
                                if(inBR)
                                {
                                    nodes[i]->getAllObjectsWithin(fill, _x, _y, _width, _height);
                                }
                                break;
                        }
                    }
                }
                // This request fits within a child so re-request from child
                else
                {
                    nodes[childIndex]->getAllObjectsWithin(fill, _x, _y, _width, _height);
                }
            }
            else
            {
                // No child split nodes, so give them my content
                getAllObjects(fill);
            }
        }
    };
}

#endif