#include "Mission.hpp"
#include "Requirement.hpp"
#include "../../bitengine/Network.hpp"
#include "../GameEvent.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"


Mission::Mission()
    : id(0), parentCharacter(NULL), parentMission(NULL), isComplete(false), generationType(GenerationType::Scripted), logicalType(LogicalType::Sequence)
{
}

Mission::~Mission()
{
    for(unsigned int i=0; i < requirements.size(); i++)
    {
        delete requirements[i];
    }
}

void Mission::load(unsigned int _id, LogicalType _logicalType, GenerationType _generationType)
{
    id = _id;
    logicalType = _logicalType;
    generationType = _generationType;
}

void Mission::assignCharacter(Character* character)
{
    parentCharacter = character;
}

void Mission::assignRequirement(Requirement* requirement)
{
    requirements.push_back(requirement);
}

bool Mission::attemptCompleteMission()
{
    if(childMissions.size() > 0)
    {
        switch(logicalType)
        {
            default:
            case LogicalType::Sequence:
            {
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(!childMissions[i]->attemptCompleteMission())
                    {
                        isComplete = false;
                        return false;
                    }
                }
                sendMissionCompletePacket();
                isComplete = true;
                return true;
            }
            case LogicalType::Selector:
            {
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(!childMissions[i]->attemptCompleteMission())
                    {
                        sendMissionCompletePacket();
                        isComplete = true;
                        return true;
                    }
                }
                isComplete = false;
                return false;
            }
        }
    }

    if(isComplete)
    {
        return true;
    }

    if(areRequirementsMet())
    {
        succeed();
        isComplete = true;
        parentCharacter->missionStateChanged = true;
        sendMissionCompletePacket();
        return true;
    }

    isComplete = false;
    return false;
}

bool Mission::areRequirementsMet()
{
    switch(logicalType)
    {
        default:
        case LogicalType::Sequence:
        {
            for(unsigned int i=0; i < requirements.size(); i++)
            {
                if(!requirements[i]->checkFullfilled(parentCharacter))
                {
                    return false;
                }
            }
            return true;
        }
        case LogicalType::Selector:
        {
            for(unsigned int i=0; i < requirements.size(); i++)
            {
                if(requirements[i]->checkFullfilled(parentCharacter))
                {
                    return true;
                }
            }
            return false;
        }
    }
}

void Mission::succeed()
{
    // Character.addExperience(experience);
}

// First elements are highest parents
void Mission::fillParentList(std::vector<unsigned int> &fill)
{
    if(parentMission)
    {
        parentMission->fillParentList(fill);
    }
    // If I am a parent add my id
    if(childMissions.size() > 0)
    {
        fill.push_back(id);
    }
}

void Mission::packParentHierarchy(bit::ServerPacket &packet)
{
    std::vector<unsigned int> parents;
    fillParentList(parents);

    packet << sf::Uint32(parents.size());
    for(unsigned int i=0; i < parents.size(); i++)
    {
        packet << sf::Uint32(parents[i]);
    }
}

void Mission::sendMissionCompletePacket()
{
    if(parentCharacter->fixedState.isPlayerCharacter)
    {
        // Mission Completed Event
        Character* c = parentCharacter;
        Mission* m = this;
        c->level->server->sendEventToClient(*parentCharacter->fixedState.player->client, [m] (bit::ServerPacket &packet) {

            packet << sf::Uint32(GameEvent::MissionCompleted);
            packet << sf::Uint32(m->id);
            m->packParentHierarchy(packet);

        });
    }
}

void Mission::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << isComplete;
    packet << sf::Uint32(logicalType);
    
    packet << sf::Uint32(childMissions.size());
    for(unsigned int i=0; i < childMissions.size(); i++)
    {
        packet << sf::Uint32(childMissions[i]->id);
        childMissions[i]->prepareSnapshot(packet);
    }

    packet << sf::Uint32(requirements.size());
    for(unsigned int i=0; i < requirements.size(); i++)
    {
        packet << sf::Uint32(requirements[i]->id);
        requirements[i]->prepareSnapshot(packet);
    }
}