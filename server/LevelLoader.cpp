#include "LevelLoader.hpp"
#include "../bitengine/System/tinyxml2.h"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
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

void LevelLoader::load(std::string file, unsigned int levelId)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile((resourcePath() + file).c_str());
    tinyxml2::XMLElement* levelPackNode = doc.FirstChildElement( "levelpack" );
    tinyxml2::XMLElement* levelsNode = levelPackNode->FirstChildElement( "levels" );
    tinyxml2::XMLElement* levelNode = levelsNode->FirstChildElement("level");

    // Get level dimensions
    levelNode->FirstChildElement("rows")->QueryUnsignedText(&rows);
    levelNode->FirstChildElement("columns")->QueryUnsignedText(&columns);
    size = rows * columns;

    // Parse id maps into linear arrays
    loadIdString(levelNode->FirstChildElement("tileMap")->GetText(), tileIdMap);
    loadIdString(levelNode->FirstChildElement("structureMap")->GetText(), structureIdMap);
    loadIdString(levelNode->FirstChildElement("characterMap")->GetText(), characterIdMap);
    loadIdString(levelNode->FirstChildElement("lightMap")->GetText(), lightIdMap);

    // Load definitions

    // Tiles
    tinyxml2::XMLElement* tilesNode = levelNode->FirstChildElement("tiles");
    for (tinyxml2::XMLElement* child = tilesNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Tile tileDef;
        tileDef.unpack(child);
        tileDefs.push_back(tileDef);
    }

    // Structures
    tinyxml2::XMLElement* structuresNode = levelNode->FirstChildElement("structures");
    for (tinyxml2::XMLElement* child = structuresNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Structure structureDef;
        structureDef.unpack(child);
        structureDefs.push_back(structureDef);
    }

    // Characters
    tinyxml2::XMLElement* charactersNode = levelNode->FirstChildElement("characters");
    for (tinyxml2::XMLElement* child = charactersNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Character characterDef;
        characterDef.unpack(child);
        characterDefs.push_back(characterDef);
    }

    // Lights
    tinyxml2::XMLElement* lightsNode = levelNode->FirstChildElement("lights");
    for (tinyxml2::XMLElement* child = lightsNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Light lightDef;
        lightDef.unpack(child);
        lightDefs.push_back(lightDef);
    }
}

void LevelLoader::loadIdString(const char* text, std::vector<unsigned int> &fill)
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

LevelLoader::Tile LevelLoader::getTileByIndex(unsigned int index)
{
    return getTileDefById(tileIdMap[index]);
}

LevelLoader::Structure LevelLoader::getStructureByIndex(unsigned int index)
{
    return getStructureDefById(structureIdMap[index]);
}

LevelLoader::Character LevelLoader::getCharacterByIndex(unsigned int index)
{
    return getCharacterDefById(characterIdMap[index]);
}

LevelLoader::Light LevelLoader::getLightByIndex(unsigned int index)
{
    return getLightDefById(lightIdMap[index]);
}

LevelLoader::Tile LevelLoader::getTileDefById(unsigned int id)
{
    for(unsigned int i=0; i < tileDefs.size(); i++)
        if(tileDefs[i].id == id)
            return tileDefs[i];
}
LevelLoader::Structure LevelLoader::getStructureDefById(unsigned int id)
{
    for(unsigned int i=0; i < structureDefs.size(); i++)
        if(structureDefs[i].id == id)
            return structureDefs[i];
}
LevelLoader::Character LevelLoader::getCharacterDefById(unsigned int id)
{
    for(unsigned int i=0; i < characterDefs.size(); i++)
        if(characterDefs[i].id == id)
            return characterDefs[i];
}
LevelLoader::Light LevelLoader::getLightDefById(unsigned int id)
{
    for(unsigned int i=0; i < lightDefs.size(); i++)
        if(lightDefs[i].id == id)
            return lightDefs[i];
}