#include "LevelLoader.hpp"
#include "../bitengine/System/tinyxml2.h"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include <sstream>

void LevelLoader::Tile::unpack(tinyxml2::XMLElement* node)
{
    node->FirstChildElement("id")->QueryUnsignedText(&id);
    node->FirstChildElement("type")->QueryUnsignedText(&type);
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
    tinyxml2::XMLElement* charactersNode = levelNode->FirstChildElement("characters");
    for (tinyxml2::XMLElement* child = charactersNode->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        LevelLoader::Character characterDef;
        characterDef.unpack(child);
        characterDefs.push_back(characterDef);
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
