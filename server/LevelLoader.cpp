#include "LevelLoader.hpp"
#include "../bitengine/System/tinyxml2.h"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "../xogeni/LevelGenerator/LevelGenerator.hpp"
#include "../xogeni/LevelGenerator/CellMap.hpp"
#include "../xogeni/LevelGenerator/Cell.hpp"
#include <sstream>

void LevelLoader::Event::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("type")->QueryUnsignedText(&type);
    node->FirstChildElement("targetLevelId")->QueryUnsignedText(&targetLevelId);
    node->FirstChildElement("targetEntranceId")->QueryUnsignedText(&targetEntranceId);
}

void LevelLoader::Entrance::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("priority")->QueryUnsignedText(&priority);
}

void LevelLoader::Tile::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("type")->QueryUnsignedText(&type);

    // Enter events in tile
    tinyxml2::XMLElement* enterEventNodes = node->FirstChildElement("enterEvents");
    for (tinyxml2::XMLElement* child = enterEventNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Event eventDef;
        eventDef.unpack(child);
        enterEvents.push_back(eventDef);
    }

    // Exit events in tile
    tinyxml2::XMLElement* exitEventNodes = node->FirstChildElement("exitEvents");
    for (tinyxml2::XMLElement* child = exitEventNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Event eventDef;
        eventDef.unpack(child);
        exitEvents.push_back(eventDef);
    }
    
    // Entrances in tile
    tinyxml2::XMLElement* entranceNodes = node->FirstChildElement("entrances");
    for (tinyxml2::XMLElement* child = entranceNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Entrance entranceDef;
        entranceDef.unpack(child);
        entrances.push_back(entranceDef);
    }
}

void LevelLoader::Structure::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("type")->QueryUnsignedText(&type);
    node->FirstChildElement("isOpen")->QueryBoolText(&isOpen);
    node->FirstChildElement("isLocked")->QueryBoolText(&isLocked);

    // Items in structure
    tinyxml2::XMLElement* itemNodes = node->FirstChildElement("items");
    for (tinyxml2::XMLElement* child = itemNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Item itemDef;
        itemDef.unpack(child);
        items.push_back(itemDef);
    }

    // Lights in structure
    tinyxml2::XMLElement* lightNodes = node->FirstChildElement("lights");
    for (tinyxml2::XMLElement* child = lightNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Light lightDef;
        lightDef.unpack(child);
        lights.push_back(lightDef);
    }
}

void LevelLoader::Character::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("type")->QueryUnsignedText(&type);

    // Items in character
    tinyxml2::XMLElement* itemNodes = node->FirstChildElement("items");
    for (tinyxml2::XMLElement* child = itemNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Item itemDef;
        itemDef.unpack(child);
        items.push_back(itemDef);
    }

    // Lights in character
    tinyxml2::XMLElement* lightNodes = node->FirstChildElement("lights");
    for (tinyxml2::XMLElement* child = lightNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Light lightDef;
        lightDef.unpack(child);
        lights.push_back(lightDef);
    }
    
    // Special equipment on character
    tinyxml2::XMLElement* equipmentNodes = node->FirstChildElement("equipment");
    for (tinyxml2::XMLElement* child = equipmentNodes->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Item itemDef;
        itemDef.unpack(child);
        equipment.push_back(itemDef);
    }
}

void LevelLoader::Item::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("type")->QueryUnsignedText(&type);
    node->FirstChildElement("slot")->QueryUnsignedText(&slot);
    node->FirstChildElement("position")->QueryUnsignedText(&position);
}

void LevelLoader::Light::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("radius")->QueryUnsignedText(&radius);
    node->FirstChildElement("red")->QueryUnsignedText(&red);
    node->FirstChildElement("green")->QueryUnsignedText(&green);
    node->FirstChildElement("blue")->QueryUnsignedText(&blue);
    node->FirstChildElement("brightness")->QueryFloatText(&brightness);
}

void LevelLoader::Level::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("defaultEntranceId")->QueryUnsignedText(&defaultEntranceId);

    // Get level dimensions
    node->FirstChildElement("rows")->QueryUnsignedText(&rows);
    node->FirstChildElement("columns")->QueryUnsignedText(&columns);
    size = rows * columns;

    // Parse id maps into linear arrays
    loadIdString(node->FirstChildElement("tileMap")->GetText(), tileIdMap);
    loadIdString(node->FirstChildElement("structureMap")->GetText(), structureIdMap);
    loadIdString(node->FirstChildElement("characterMap")->GetText(), characterIdMap);
    loadIdString(node->FirstChildElement("lightMap")->GetText(), lightIdMap);

    // Load definitions

    // Tiles
    tinyxml2::XMLElement* tilesNode = node->FirstChildElement("tiles");
    for (tinyxml2::XMLElement* child = tilesNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Tile tileDef;
        tileDef.unpack(child);
        tileDefs.push_back(tileDef);
    }

    // Structures
    tinyxml2::XMLElement* structuresNode = node->FirstChildElement("structures");
    for (tinyxml2::XMLElement* child = structuresNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Structure structureDef;
        structureDef.unpack(child);
        structureDefs.push_back(structureDef);
    }

    // Characters
    tinyxml2::XMLElement* charactersNode = node->FirstChildElement("characters");
    for (tinyxml2::XMLElement* child = charactersNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Character characterDef;
        characterDef.unpack(child);
        characterDefs.push_back(characterDef);
    }

    // Lights
    tinyxml2::XMLElement* lightsNode = node->FirstChildElement("lights");
    for (tinyxml2::XMLElement* child = lightsNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Light lightDef;
        lightDef.unpack(child);
        lightDefs.push_back(lightDef);
    }
}

void LevelLoader::Level::loadIdString(const char* text, std::vector<unsigned int> &fill)
{
    std::string s(text);
    std::vector<std::string> vs;
    bit::String::split(s, ',', vs);
    for(unsigned int i=0; i < vs.size(); i++)
    {
        unsigned int id;
        std::istringstream(vs[i]) >> id;
        fill.push_back(id);
    }
}

LevelLoader::Tile LevelLoader::Level::getTileByIndex(unsigned int index)
{
    return getTileDefById(tileIdMap[index]);
}

LevelLoader::Structure LevelLoader::Level::getStructureByIndex(unsigned int index)
{
    return getStructureDefById(structureIdMap[index]);
}

LevelLoader::Character LevelLoader::Level::getCharacterByIndex(unsigned int index)
{
    return getCharacterDefById(characterIdMap[index]);
}

LevelLoader::Light LevelLoader::Level::getLightByIndex(unsigned int index)
{
    return getLightDefById(lightIdMap[index]);
}

LevelLoader::Tile LevelLoader::Level::getTileDefById(unsigned int id)
{
    for(unsigned int i=0; i < tileDefs.size(); i++)
        if(tileDefs[i].id == id)
            return tileDefs[i];
}
LevelLoader::Structure LevelLoader::Level::getStructureDefById(unsigned int id)
{
    for(unsigned int i=0; i < structureDefs.size(); i++)
        if(structureDefs[i].id == id)
            return structureDefs[i];
}
LevelLoader::Character LevelLoader::Level::getCharacterDefById(unsigned int id)
{
    for(unsigned int i=0; i < characterDefs.size(); i++)
        if(characterDefs[i].id == id)
            return characterDefs[i];
}
LevelLoader::Light LevelLoader::Level::getLightDefById(unsigned int id)
{
    for(unsigned int i=0; i < lightDefs.size(); i++)
        if(lightDefs[i].id == id)
            return lightDefs[i];
}


void LevelLoader::loadFromXML(std::string file)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile((resourcePath() + file).c_str());
    tinyxml2::XMLElement* levelPackNode = doc.FirstChildElement( "levelpack" );

    levelPackNode->FirstChildElement("defaultLevelId")->QueryUnsignedText(&defaultLevelId);

    // Levels
    tinyxml2::XMLElement* levelsNode = levelPackNode->FirstChildElement("levels");
    for (tinyxml2::XMLElement* child = levelsNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Level levelDef;
        levelDef.unpack(child);
        levelDefs.push_back(levelDef);
    }
}
 
void LevelLoader::loadFromXoGeni(XoGeni::LevelGenerator &levelGenerator, unsigned int seed, unsigned int width, unsigned int height)
{
    unsigned int levelId = 1;
    for(unsigned int i=0; i < 10; i++)
    {
        XoGeni::CellMap* cellMap = levelGenerator.generate(seed, width, height, levelId);

        LevelLoader::Level levelDef;
        levelDef.unpack(cellMap, levelId);
        levelDefs.push_back(levelDef);
        levelId++;

        delete cellMap;
    }
}

void LevelLoader::Level::unpack(XoGeni::CellMap* cellMap, unsigned int levelId)
{
    id = levelId;
    defaultEntranceId = 1;

    // Get level dimensions
    rows = cellMap->width;
    columns = cellMap->height;
    size = rows * columns;

    // Parse id maps into linear arrays
    // no structureIdMap, characterIdMap, lightIdMap yet
    unsigned int tId = 1;
    unsigned int sId = 1;
    unsigned int lId = 1;
    for(unsigned int i=0; i < cellMap->cells.size(); i++)
    {
        XoGeni::Cell* cell = cellMap->cells[i];

        // Tiles
        tileIdMap.push_back(tId);
        LevelLoader::Tile tileDef;
        tileDef.unpack(cellMap->cells[i], tId);
        tileDefs.push_back(tileDef);
        tId++;


        // Structures
        if(cell->isWall || cell->isDoor)
        {
            structureIdMap.push_back(sId);
            LevelLoader::Structure structureDef;
            structureDef.unpack(cell, sId);
            structureDefs.push_back(structureDef);
            sId++;
        }
        else
        {
            structureIdMap.push_back(0);
        }

        // Lights
        if(cell->hasLight)
        {
            lightIdMap.push_back(lId);
            LevelLoader::Light lightDef;
            lightDef.unpack(cell, lId);
            lightDefs.push_back(lightDef);
            lId++;
        }
        else
        {
            lightIdMap.push_back(0);
        }

        // Characters
        characterIdMap.push_back(0);

    }
}

void LevelLoader::Tile::unpack(XoGeni::Cell* cell, unsigned int tileId)
{
    id = tileId;
    if(cell->isEntrance)
        type = 4; // StairwellUp_North
    else if(cell->isExit)
        type = 2; // StairwellDown_South
    else
        type = 1; // Ground

    // Enter events in tile

    // Exit events in tile
    
    // Entrances in tile
    if(cell->isEntrance)
    {
        LevelLoader::Entrance entranceDef;
        entranceDef.id = cell->entranceId;
        entranceDef.priority = 1;
        entrances.push_back(entranceDef);
    }
}

void LevelLoader::Structure::unpack(XoGeni::Cell* cell, unsigned int structureId)
{
    id = structureId;
    type = cell->isWall ? 1 : 2; // wall or door currently
    isOpen = false;
    isLocked = false;

    // Items in structure

    // Lights in structure
}

void LevelLoader::Light::unpack(XoGeni::Cell* cell, unsigned int lightId)
{
    id = lightId;
    radius = cell->lightRadius;
    brightness = cell->lightOpacity;
    red = cell->lightColorR;
    green = cell->lightColorG;
    blue = cell->lightColorB;
}
