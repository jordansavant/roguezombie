#pragma once
#ifndef RZ_LEVELLOADER_H
#define RZ_LEVELLOADER_H

class LevelLoader
{
public:
    struct Tile
    {
        unsigned int id;
        unsigned int type;
    };
    struct Item
    {
        unsigned int id;
        unsigned int type;
        unsigned int slot;
        unsigned int position;
    };
    struct Light
    {
        unsigned int id;
        unsigned int radius;
        unsigned int red;
        unsigned int green;
        unsigned int blue;
        float brightness;
    };
    struct Structure
    {
        unsigned int id;
        unsigned int type;
        bool isOpen;
        bool isLocked;
        std::vector<Item> items;
        std::vector<Light> lights;
    };
    struct Character
    {
        unsigned int id;
        unsigned int type;
        std::vector<Item> items;
        std::vector<Light> lights;
    };
};

#endif