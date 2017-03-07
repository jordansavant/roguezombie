#pragma once
#ifndef BIT_QUADTREEOBJECT_H
#define BIT_QUADTREEOBJECT_H

namespace bit
{
    class QuadTreeObject
    {
    public:
        
        virtual float quadtree_getX() = 0;
        
        virtual float quadtree_getY() = 0;

        virtual float quadtree_getWidth() = 0;

        virtual float quadtree_getHeight() = 0;

    };
}
#endif