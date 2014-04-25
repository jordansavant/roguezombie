#include "Mission.hpp"
#include "../../bitengine/Network.hpp"
#include "../ServerEvent.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"

Mission::Mission()
    : schema(), parentCharacter(NULL), parentMission(NULL), requirement(NULL)
{
}

Mission::~Mission()
{
    for(unsigned int i=0; i < childMissions.size(); i++)
    {
        delete childMissions[i];
    }
}

void Mission::load(unsigned int _id, LogicalType _logicalType, GenerationType _generationType, JournalEntry::Entry _journalEntry)
{
    schema.id = _id;
    schema.logicalType = _logicalType;
    schema.generationType = _generationType;
    schema.journalEntry = _journalEntry;
}

void Mission::assignCharacter(Character* character)
{
    parentCharacter = character;
}

void Mission::assignChildMission(Mission* mission)
{
    childMissions.push_back(mission);
    mission->parentMission = this;
}

void Mission::assignRequirement(std::function<bool(Character*)> _requirement)
{
    requirement = _requirement;
}

bool Mission::attemptCompleteMission()
{
    if(schema.isComplete)
    {
        return true;
    }

    if(childMissions.size() > 0)
    {
        switch(schema.logicalType)
        {
            default:
            case LogicalType::Sequence:
            {
                // All children must be completed in order for me to be completed
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(!childMissions[i]->attemptCompleteMission())
                    {
                        schema.isComplete = false;
                        return false;
                    }
                }
                schema.isComplete = true;
                onMissionComplete();
                return true;
            }
            case LogicalType::Selector:
            {
                // Any child must be completed in order for me to be completed
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(childMissions[i]->attemptCompleteMission())
                    {
                        schema.isComplete = true;
                        onMissionComplete();
                        return true;
                    }
                }
                schema.isComplete = false;
                return false;
            }
        }
    }

    // If I am a leaf mission, check my requirements
    if(isRequirementFulfilled())
    {
        succeed();
        schema.isComplete = true;
        onMissionComplete();
        return true;
    }

    schema.isComplete = false;
    return false;
}

bool Mission::isRequirementFulfilled()
{
    return requirement(getParentCharacter());
}

void Mission::succeed()
{
    // Character.addExperience(experience);
}

Character* Mission::getParentCharacter()
{
    if(parentMission)
    {
        return parentMission->getParentCharacter();
    }

    return parentCharacter;
}

void Mission::onMissionComplete()
{
    // Mission Completed Event Packet
    Character* c = getParentCharacter();
    if(c->schema.isPlayerCharacter)
    {
        c->level->server->sendEventToClient(*c->schema.player->client, std::bind(&Mission::prepareServerEventPacket_missionComplete, this, std::placeholders::_1));
    }
}

// First elements are highest parents
void Mission::fillIdHierarchy(std::vector<unsigned int> &fill)
{
    if(parentMission)
    {
        parentMission->fillIdHierarchy(fill);
    }
    fill.push_back(schema.id);
}

void Mission::packIdHierarchy(bit::ServerPacket &packet)
{
    std::vector<unsigned int> parents;
    fillIdHierarchy(parents);

    packet << sf::Uint32(parents.size());
    for(unsigned int i=0; i < parents.size(); i++)
    {
        packet << sf::Uint32(parents[i]);
    }
}

void Mission::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << schema;

    packet << sf::Uint32(childMissions.size());
    for(unsigned int i=0; i < childMissions.size(); i++)
    {
        packet << sf::Uint32(childMissions[i]->schema.id);
        childMissions[i]->prepareSnapshot(packet);
    }
}

void Mission::prepareServerEventPacket_missionComplete(bit::ServerPacket &packet)
{
    packet << sf::Uint32(ServerEvent::MissionCompleted);
    packIdHierarchy(packet);
}
