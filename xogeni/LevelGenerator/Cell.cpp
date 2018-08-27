#include "Cell.hpp"
#include "Room.hpp"

#include <stddef.h>

XoGeni::Cell::Cell(unsigned int index, unsigned int x, unsigned int y)
    : index(index), x(x), y(y), room(NULL), tunnelTestCount(0),
    isWall(false), isRoomEdge(false), isRoomPermiter(false), isSill(false),
    isDoor(false), wasDoor(false),
    isTunnel(false), wasCorridorTunnel(false), wasDoorTunnel(false), wasRoomFixTunnel(false),
    isEntrance(false), entranceId(0), entrancePriority(0),
    isEntranceTransition(false), entranceTransition(NULL),
    isExitTransition(false), exitTransition(NULL), transitionDir(0, 0),
    hasLight(false), lightRadius(0), lightOpacity(0), lightColorR(0), lightColorG(0), lightColorB(0),
    isTagUnreachable(false),
    hasCharacter(false), characterType(0), hasCharacterWithKey(false), characterKeyAccessLevel(0),
    hasStructure(false), structureType(0), structureSubType(0), structureAccessLevel(0), isLocked(false),
    metadata_floodfillId(0)
{
    aStarX = x;
    aStarY = y;
}

bool XoGeni::Cell::isOccupied()
{
    return isTagUnreachable || isWall || isDoor || hasCharacter || hasStructure || isEntrance || isEntranceTransition || isExitTransition;
}



void XoGeni::Cell::decorate(unsigned int type, unsigned int subtype)
{
    structureType = type;
    structureSubType = subtype;

    switch (type)
    {
        case 3: // chest
            break;
        case 4: // furnishings
        {
            switch (subtype)
            {
                case 1: // Terminal facing South
                    hasLight = true;
                    lightColorR = 200;
                    lightColorG = 255;
                    lightColorB = 200;
                    lightRadius = 1;
                    lightOpacity = 0.2;
                    break;
                case 2: // Column
                    break;
                case 3: // Terminal East
                    hasLight = true;
                    lightColorR = 200;
                    lightColorG = 255;
                    lightColorB = 200;
                    lightRadius = 1;
                    lightOpacity = 0.2;
                    break;
                case 4: // Desk South
                    break;
                case 5: // Desk East
                    break;
                case 6: // Sink South
                    break;
                case 7: // Sink East
                    break;
                case 8: // Optable
                    hasLight = true;
                    lightColorR = 255;
                    lightColorG = 255;
                    lightColorB = 255;
                    lightRadius = 2;
                    lightOpacity = 0.75;
                    break;
                case 9: // Pilon
                    hasLight = true;
                    lightColorR = 200;
                    lightColorG = 200;
                    lightColorB = 255;
                    lightRadius = 2;
                    lightOpacity = .2;
                    break;
            }
            break;
        }
    }
}