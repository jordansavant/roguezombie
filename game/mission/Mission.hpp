#pragma once
#ifndef RZ_MISSION_H
#define RZ_MISSION_H

#include <vector>
#include <functional>
#include "../../bitengine/Network.hpp"
#include "LogicalType.hpp"
#include "JournalEntry.hpp"

class Character;

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
    JournalEntry::Entry journalEntry;

    void load(unsigned int id, LogicalType logicalType, GenerationType generationType, JournalEntry::Entry journalEntry);

    void assignCharacter(Character* character);

    void assignChildMission(Mission* mission);

    void assignRequirement(std::function<bool(Character*)> _requirement);

    bool attemptCompleteMission();

    void succeed();

    Character* getParentCharacter();

    void fillIdHierarchy(std::vector<unsigned int> &fill);

    virtual void packIdHierarchy(bit::ServerPacket &packet);

    virtual void sendMissionCompletePacket();

    virtual void prepareSnapshot(bit::ServerPacket &packet);

private:

    Mission* parentMission;
    std::vector<Mission*> childMissions;
    std::function<bool(Character*)> requirement;

    bool attemptCompleteMission(Mission* mission);

    bool isRequirementFulfilled();
};

#endif