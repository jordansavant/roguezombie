#pragma once
#ifndef BIT_QUADTREE_H
#define BIT_QUADTREE_H

#include <vector>
#include <iostream>
#include <sstream>

namespace bit
{
    class QuadTreeObject;

    class QuadTree
    {
    public:
        QuadTree(float x, float y, float width, float height, int level = 0, int poolSize = 100);

        ~QuadTree();

        int maxObjects;
        int maxLevels;
        int level;
        float x, y;
        float width, height;
        std::vector<QuadTreeObject*> objects;
        std::vector<QuadTree*>* pool;
        std::vector<QuadTree*> nodes;

        void clear();

        void split();

        QuadTree* fetchFromPool(float _x, float _y, float _width, float _height, int _level);

        void returnToPool(QuadTree* quadTree);

        int getIndex(float _x, float _y, float _width, float _height);

        int getIndex(QuadTreeObject* pRect);

        void insert(QuadTreeObject* pRect);

        void getNeighbors(std::vector<QuadTreeObject*> &fill, QuadTreeObject* pRect);

        void getObjectsNear(std::vector<QuadTreeObject*> &fill, float x, float y, float width, float height);
    };
}

#endif