#pragma once
#ifndef RZ_REQUIREMENT_H
#define RZ_REQUIREMENT_H

#include <functional>
#include "../../bitengine/Network.hpp"
#include "LogicalType.hpp"
#include "JournalEntry.hpp"

class Character;

class Requirement
{
public:

    Requirement();
    
    enum GenerationType
    {
        Scripted,
        Generated
    };

    unsigned int id;
    std::function<bool(Character*)> check;
    bool isFullfilled;
    GenerationType generationType;
    // unsigned int phoeneticVerbId; // if Generated
    // unsigned int phoeneticNounId; // if Generated
    JournalEntry::Entry journalEntry;

    void load(unsigned int id, GenerationType generationType, JournalEntry::Entry journalEntry, std::function<bool(Character*)> check);

    bool checkFullfilled(Character* c);

    virtual void prepareSnapshot(bit::ServerPacket &packet);
};

#endif