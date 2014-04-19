#include "Mission.hpp"
#include "../../bitengine/Network.hpp"
#include "../GameEvent.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"


Mission::Mission()
    : id(0), parentCharacter(NULL), parentMission(NULL), isComplete(false), generationType(GenerationType::Scripted), logicalType(LogicalType::Sequence), journalEntry(JournalEntry::Entry::None), requirement(NULL)
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
    id = _id;
    logicalType = _logicalType;
    generationType = _generationType;
    journalEntry = _journalEntry;
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
    if(isComplete)
    {
        return true;
    }

    if(childMissions.size() > 0)
    {
        switch(logicalType)
        {
            default:
            case LogicalType::Sequence:
            {
                // All children must be completed in order for me to be completed
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
                // Any child must be completed in order for me to be completed
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(childMissions[i]->attemptCompleteMission())
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

    // If I am a leaf mission, check my requirements
    if(isRequirementFulfilled())
    {
        succeed();
        isComplete = true;
        getParentCharacter()->missionStateChanged = true;
        sendMissionCompletePacket();
        return true;
    }

    isComplete = false;
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
    Character* c = getParentCharacter();
    if(c->fixedState.isPlayerCharacter)
    {
        // Mission Completed Event
        Mission* m = this;
        c->level->server->sendEventToClient(*c->fixedState.player->client, [m] (bit::ServerPacket &packet) {

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
    packet << sf::Uint32(journalEntry);
    
    packet << sf::Uint32(childMissions.size());
    for(unsigned int i=0; i < childMissions.size(); i++)
    {
        packet << sf::Uint32(childMissions[i]->id);
        childMissions[i]->prepareSnapshot(packet);
    }
}