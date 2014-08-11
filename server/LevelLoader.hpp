#pragma once
#ifndef RZ_LEVELLOADER_H
#define RZ_LEVELLOADER_H

#include <string>
#include <vector>
#include "../bitengine/System/tinyxml2.h"

class LevelLoader
{
public:

    struct Tile
    {
        unsigned int id;
        unsigned int type;

        void unpack(tinyxml2::XMLElement* node);
    };
    
    struct Item
    {
        unsigned int id;
        unsigned int type;
        unsigned int slot;
        unsigned int position;

        void unpack(tinyxml2::XMLElement* node);
    };
    
    struct Light
    {
        unsigned int id;
        unsigned int radius;
        unsigned int red;
        unsigned int green;
        unsigned int blue;
        float brightness;

        void unpack(tinyxml2::XMLElement* node);
    };
    
    struct Structure
    {
        unsigned int id;
        unsigned int type;
        bool isOpen;
        bool isLocked;
        std::vector<Item> items;
        std::vector<Light> lights;

        void unpack(tinyxml2::XMLElement* node);
    };

    struct Character
    {
        unsigned int id;
        unsigned int type;
        std::vector<Item> items;
        std::vector<Light> lights;

        void unpack(tinyxml2::XMLElement* node);
    };

    unsigned int levelId;
    std::string title;
    unsigned int rows;
    unsigned int columns;
    unsigned int size;
    std::vector<unsigned int> tileIdMap;
    std::vector<unsigned int> structureIdMap;
    std::vector<unsigned int> characterIdMap;
    std::vector<unsigned int> lightIdMap;
    std::vector<Tile> tileDefs;
    std::vector<Structure> structureDefs;
    std::vector<Character> characterDefs;
    std::vector<Light> lightDefs;

    void load(std::string file, unsigned int levelId);

    void loadIdString(const char* text, std::vector<unsigned int> &fill);

    LevelLoader::Tile getTileByIndex(unsigned int index);

    LevelLoader::Structure getStructureByIndex(unsigned int index);

    LevelLoader::Character getCharacterByIndex(unsigned int index);

    LevelLoader::Light getLightByIndex(unsigned int index);

    LevelLoader::Tile getTileDefById(unsigned int id);

    LevelLoader::Structure getStructureDefById(unsigned int id);

    LevelLoader::Character getCharacterDefById(unsigned int id);

    LevelLoader::Light getLightDefById(unsigned int id);

};

#endif