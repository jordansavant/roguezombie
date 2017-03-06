#pragma once
#ifndef RZ_LEVELLOADER_H
#define RZ_LEVELLOADER_H

#include <string>
#include <vector>
#include "../bitengine/System/tinyxml2.h"
#include "../xogeni/LevelGenerator/LevelGenerator.hpp"

namespace XoGeni {
    class CellMap;
    class Cell;
}

class LevelLoader
{
public:

    struct Event
    {
        enum Type
        {
            undefined,
            PlayerGoToLevel,
            NpcGoToLevel,
            GameVictory,
            GameDefeat,
        };

        unsigned int type;
        unsigned int targetLevelId;
        unsigned int targetEntranceId;

        void unpack(tinyxml2::XMLElement* node);

    };

    struct Entrance
    {
        unsigned int id;
        unsigned int priority;

        void unpack(tinyxml2::XMLElement* node);
    };

    struct Tile
    {
        unsigned int id;
        unsigned int type;
        std::vector<Event> enterEvents;
        std::vector<Event> exitEvents;
        std::vector<Entrance> entrances;

        void unpack(tinyxml2::XMLElement* node);

        void unpack(XoGeni::Cell* cell, unsigned int id);
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

        void unpack(XoGeni::Cell* cell, unsigned int id);
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

        void unpack(XoGeni::Cell* cell, unsigned int id);
    };

    struct Character
    {
        unsigned int id;
        unsigned int type;
        std::vector<Item> items;
        std::vector<Light> lights;
        std::vector<Item> equipment;

        void unpack(tinyxml2::XMLElement* node);
    };

    struct Level
    {
        unsigned int id;
        std::string title;
        unsigned int rows;
        unsigned int columns;
        unsigned int size;
        unsigned int defaultEntranceId;
        std::vector<unsigned int> tileIdMap;
        std::vector<unsigned int> structureIdMap;
        std::vector<unsigned int> characterIdMap;
        std::vector<unsigned int> lightIdMap;
        std::vector<Tile> tileDefs;
        std::vector<Structure> structureDefs;
        std::vector<Character> characterDefs;
        std::vector<Light> lightDefs;

        void unpack(tinyxml2::XMLElement* node);

        void unpack(XoGeni::CellMap* cellMap);

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

    unsigned int version;
    std::string name;
    unsigned int defaultLevelId;
    std::vector<Level> levelDefs;

    void loadFromXML(std::string file);

    void loadFromXoGeni(XoGeni::LevelGenerator &levelGenerator, unsigned int seed, unsigned int width, unsigned int height);

};

#endif