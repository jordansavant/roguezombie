#include "QuadTree.hpp"
#include "QuadTreeObject.hpp"

bit::QuadTree::QuadTree(float x, float y, float width, float height, int level, int poolSize)
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

bit::QuadTree::~QuadTree()
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

// Clears the QuadTree
void bit::QuadTree::clear()
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

// Splits the node into 4 subnodes
void bit::QuadTree::split()
{
    float subWidth =  width / 2;
    float subHeight = height / 2;
    float subX = x;
    float subY = y;

    nodes.push_back(fetchFromPool(subX + subWidth, subY, subWidth, subHeight, level + 1));
    nodes.push_back(fetchFromPool(subX, subY, subWidth, subHeight, level + 1));
    nodes.push_back(fetchFromPool(subX, subY + subHeight, subWidth, subHeight, level + 1));
    nodes.push_back(fetchFromPool(subX + subWidth, subY + subHeight, subWidth, subHeight, level + 1));
}

bit::QuadTree* bit::QuadTree::fetchFromPool(float _x, float _y, float _width, float _height, int _level)
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

void bit::QuadTree::returnToPool(QuadTree* quadTree)
{
    quadTree->level = -1;
    quadTree->x = 0;
    quadTree->y = 0;
    quadTree->width = 0;
    quadTree->height = 0;
    quadTree->pool = NULL;
    pool->push_back(quadTree);
}

// Determine which node the object belongs to. -1 means
// object cannot completely fit within a child node and is part
// of the parent node
int bit::QuadTree::getIndex(float _x, float _y, float _width, float _height)
{
    double verticalMidpoint = x + (width / 2);
    double horizontalMidpoint = y + (height / 2);

    // Object can completely fit within the top quadrants
    bool topQuadrant = (_y + _height < horizontalMidpoint);

    // Object can completely fit within the bottom quadrants
    bool bottomQuadrant = (_y > horizontalMidpoint);

    // Object can completely fit within the left quadrants
    if (_x + _width < verticalMidpoint)
    {
        if (topQuadrant)
            return 1;
        else if (bottomQuadrant)
            return 2;
    }
    // Object can completely fit within the right quadrants
    else if (_x > verticalMidpoint)
    {
        if (topQuadrant)
            return 0;
        else if (bottomQuadrant)
            return 3;
    }

    // Object does not fit, must belong to parent
    return -1;
}

int bit::QuadTree::getIndex(bit::QuadTreeObject* pRect)
{
    return getIndex(pRect->quadtree_getX(), pRect->quadtree_getY(), pRect->quadtree_getWidth(), pRect->quadtree_getHeight());
}

// Insert the object into the quadtree. If the node
// exceeds the capacity, it will split and add all
// objects to their corresponding nodes.
void bit::QuadTree::insert(QuadTreeObject* pRect)
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
                QuadTreeObject* ob = objects.at(i);
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

void bit::QuadTree::getNeighbors(std::vector<QuadTreeObject*> &fill, QuadTreeObject* pRect)
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

void bit::QuadTree::getObjectsNear(std::vector<QuadTreeObject*> &fill, float _x, float _y, float _width, float _height)
{
    // Search into children to retrieve
    int index = getIndex(_x, _y, _width, _height);
    if (index != -1 && nodes.size() > 0)
    {
        nodes[index]->getObjectsNear(fill, _x, _y, _width, _height);
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