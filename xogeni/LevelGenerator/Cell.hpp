#pragma once
#ifndef XG_CELL_H
#define XG_CELL_H

#include "../../bitengine/Intelligence.hpp"

namespace XoGeni
{
    class Room;
    class RoomDoor;
    class Entrance;
    class Exit;

    class Cell : public bit::AstarNode
    {
    public:

        Cell(unsigned int index, unsigned int x, unsigned int y);

        struct SillData
        {
            SillData(): doorX(0), doorY(0) {}
            unsigned int doorX;
            unsigned int doorY;
        };

        unsigned int index;
        unsigned int x, y;
        unsigned int tunnelTestCount;
        bool isWall;
        
        Room* room;
        bool isRoomEdge;
        bool isRoomPermiter;
        
        RoomDoor* door;
        SillData sillData;
        bool isSill;
        bool isDoor;
        bool wasDoor;
        
        bool isTunnel;
        bool wasCorridorTunnel;
        bool wasDoorTunnel;
        bool wasRoomFixTunnel;
        
        bool isEntrance;
        unsigned int entranceId;
        unsigned int entrancePriority;

        bool isEntranceTransition;
        Entrance* entranceTransition;
        bool isExitTransition;
        Exit* exitTransition;
        sf::Vector2i transitionDir;

        bool hasLight;
        unsigned int lightRadius;
        float lightOpacity;
        unsigned int lightColorR;
        unsigned int lightColorG;
        unsigned int lightColorB;

        bool isTagUnreachable;

        bool hasCharacter;
        unsigned int characterType;

        bool hasStructure;
        unsigned int structureType;
        unsigned int structureSubType;

        unsigned int metadata_floodfillId;

        bool isOccupied();

    };
}

#endif