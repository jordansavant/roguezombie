#include "Mission.hpp"
#include "Requirement.hpp"

Mission::Mission()
{
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