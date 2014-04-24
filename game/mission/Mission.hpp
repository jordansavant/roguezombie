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

    struct Schema
    {
        Schema()
            : id(0), isComplete(false), generationType(GenerationType::Scripted), logicalType(LogicalType::Sequence), experience(0), journalEntry(JournalEntry::Entry::None)
        {
        }

        unsigned int id;
        bool isComplete;
        GenerationType generationType;
        LogicalType logicalType;
        int experience;
        JournalEntry::Entry journalEntry;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << schema.isComplete;
            packet << sf::Uint32(schema.logicalType);
            packet << sf::Uint32(schema.journalEntry);
            return packet;
        }

        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.isComplete;
            bit::NetworkHelper::unpackEnum<sf::Uint32, LogicalType>(packet, schema.logicalType);
            bit::NetworkHelper::unpackEnum<sf::Uint32, JournalEntry::Entry>(packet, schema.journalEntry);
            return packet;
        }
    };
    Schema schema;
    Character* parentCharacter;

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