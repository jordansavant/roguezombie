#pragma once
#ifndef RZ_MISSION_H
#define RZ_MISSION_H

#include <vector>
#include <functional>
#include "../../bitengine/Network.hpp"
#include "LogicalType.hpp"

class Character;
class Requirement;

class Mission
{
public:

    Mission();

    ~Mission();

    enum GenerationType
    {
        Scripted,
        Generated
    };

    unsigned int id;
    Character* parentCharacter;
    bool isComplete;
    GenerationType generationType;
    LogicalType logicalType;
    // int experience;
    // JournalEntry* journalEntry;

    void assignCharacter(Character* character);

    void assignRequirement(Requirement* requirement);

    bool attemptCompleteMission();

    void succeed();

    virtual void prepareSnapshot(bit::ServerPacket &packet);

private:

    Mission* parentMission;
    std::vector<Mission*> childMissions;
    std::vector<Requirement*> requirements;

    bool attemptCompleteMission(Mission* mission);

    bool areRequirementsMet();
};

#endif