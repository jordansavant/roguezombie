#pragma once
#ifndef BIT_QUADTREE_H
#define BIT_QUADTREE_H

#include <vector>
#include <iostream>
#include <sstream>
#include "QuadTreeObject.hpp"

namespace bit
{
    class Object;

    class QuadTree
    {
    public:
        QuadTree(int _level, float x, float y, float width, float height, int poolSize = 100)
            : nodes()
        {
            maxObjects = 7;
            maxLevels = 10;

            level = _level;
            bounds.left = x;
            bounds.top = y;
            bounds.width = width;
            bounds.height = height;

            shape.setPosition(x, y);
            shape.setSize(sf::Vector2f(width, height));
            shape.setFillColor(sf::Color(0, 0, 0, 0));
            shape.setOutlineThickness(2.0f);
            shape.setOutlineColor(sf::Color(64, 128, 255));

            if(level == 0)
            {
                // The root quadtree can maintain a pool
                pool = new std::vector<QuadTree*>();
                for(int i = 0; i < poolSize; i ++)
                {
                    pool->push_back(new QuadTree(-1, 0, 0, 0, 0));
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
        std::vector<QuadTreeObject*> objects;
        float x, y;
        float width, height;
        std::vector<QuadTree*>* pool;
        std::vector<QuadTree*> nodes;

        /*
         * Clears the QuadTree
         */
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

        /*
         * Splits the node into 4 subnodes
         */
        void split()
        {
            int subWidth = (int)(bounds.width / 2);
            int subHeight = (int)(bounds.height / 2);
            int x = (int)bounds.left;
            int y = (int)bounds.top;

            nodes.push_back(fetchFromPool(level+1, x + subWidth, y, subWidth, subHeight));
            nodes.push_back(fetchFromPool(level+1, x, y, subWidth, subHeight));
            nodes.push_back(fetchFromPool(level+1, x, y + subHeight, subWidth, subHeight));
            nodes.push_back(fetchFromPool(level+1, x + subWidth, y + subHeight, subWidth, subHeight));
        }

        QuadTree* fetchFromPool(int _level, float x, float y, float width, float height)
        {
            QuadTree* fromPool;

            if(pool->size() > 0)
            {
                fromPool = pool->back();
                pool->pop_back();
            }
            else
            {
                fromPool = new QuadTree(-1, 0, 0, 0, 0);
            }

            fromPool->level = _level;
            fromPool->bounds.left = x;
            fromPool->bounds.top = y;
            fromPool->bounds.width = width;
            fromPool->bounds.height = height;
            fromPool->pool = pool;

            return fromPool;
        }

        void returnToPool(QuadTree* quadTree)
        {
            quadTree->level = -1;
            quadTree->bounds.left = 0;
            quadTree->bounds.top = 0;
            quadTree->bounds.width = 0;
            quadTree->bounds.height = 0;
            quadTree->pool = NULL;
            pool->push_back(quadTree);
        }

        /*
         * Determine which node the object belongs to. -1 means
         * object cannot completely fit within a child node and is part
         * of the parent node
         */
        int getIndex(T* pRect)
        {
            int index = -1;
            double verticalMidpoint = bounds.left + (bounds.width / 2);
            double horizontalMidpoint = bounds.top + (bounds.height / 2);

            // Object can completely fit within the top quadrants
            bool topQuadrant = (pRect->box.top < horizontalMidpoint && pRect->box.top + pRect->box.height < horizontalMidpoint);

            // Object can completely fit within the bottom quadrants
            bool bottomQuadrant = (pRect->box.top > horizontalMidpoint);

            // Object can completely fit within the left quadrants
            if (pRect->box.left < verticalMidpoint && pRect->box.left + pRect->box.width < verticalMidpoint)
            {
                if (topQuadrant)
                {
                    index = 1;
                }
                else if (bottomQuadrant)
                {
                    index = 2;
                }
            }
            // Object can completely fit within the right quadrants
            else if (pRect->box.left > verticalMidpoint)
            {
                if (topQuadrant)
                {
                    index = 0;
                }
                else if (bottomQuadrant)
                {
                    index = 3;
                }
           }

           return index;
        }

        /*
        * Insert the object into the quadtree. If the node
        * exceeds the capacity, it will split and add all
        * objects to their corresponding nodes.
        */
        void insert(T* pRect)
        {
            if (nodes.size() > 0)
            {
                int index = getIndex(pRect);

                if (index != -1)
                {
                    if(index < 0 || index > nodes.size())
                    {
                       bool f = false;
                    }

                    nodes[index]->insert(pRect);

                    return;
                }
            }

            objects.push_back(pRect);

            if (objects.size() > maxObjects && level < maxLevels)
            {
                if (nodes.size() == 0)
                {
                    split();
                }

                int i = 0;
                while (i < objects.size())
                {
                    int index = getIndex(objects.at(i));

                    if (index != -1)
                    {
                        // TODO: gross clean up this
                        Mob* mob = objects.at(i);
                        objects.erase(objects.begin() + i);
                        nodes[index]->insert(mob);
                    }
                    else
                    {
                        i++;
                    }
                }
            }
        }

        /*
         * Return all objects that could collide with the given object
         */
        std::vector<T*>* retrieve(std::vector<T*>* returnObjects, T* pRect)
        {
            int index = getIndex(pRect);

            if (index != -1 && nodes.size() > 0)
            {
                nodes[index]->retrieve(returnObjects, pRect);
            }

            for(int i = 0; i < objects.size(); i++)
            {
                returnObjects->push_back(objects[i]);
            }

            return returnObjects;
        }

        void Draw(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera)
        {
            shape.setSize(sf::Vector2f(bounds.width, bounds.height));
            shape.setPosition(camera.translateXToCameraX(bounds.left), camera.translateYToCameraY(bounds.top));

            //std::stringstream ss;
            //ss << objects.size();
            //std::string numObjectsStr = ss.str();
            window.draw(shape);

            for(int i = 0; i < nodes.size(); i++)
            {
                if(nodes[i])
                {
                    nodes[i]->Draw(window, gameTime, camera);
                }
            }
        }
    };
}

#endif