#include "Mission.hpp"
#include "Requirement.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

Mission::Mission()
    : id(0), parentCharacter(NULL), isComplete(false), generationType(GenerationType::Scripted), logicalType(LogicalType::Sequence)
{
}

Mission::~Mission()
{
    for(unsigned int i=0; i < requirements.size(); i++)
    {
        delete requirements[i];
    }
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
                isComplete = true;
                return true;
            }
            case LogicalType::Selector:
            {
                for(unsigned int i=0; i < childMissions.size(); i++)
                {
                    if(!childMissions[i]->attemptCompleteMission())
                    {
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