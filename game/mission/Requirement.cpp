#include "Requirement.hpp"
#include "../Character.hpp"
#include "../../bitengine/Network.hpp"

Requirement::Requirement()
    : id(0), check(NULL), isFullfilled(false), generationType(GenerationType::Scripted), journalEntry(JournalEntry::Entry::None)
{
}

void Requirement::load(unsigned int _id, GenerationType _generationType, JournalEntry::Entry _journalEntry, std::function<bool(Character*)> _check)
{
    id = _id;
    generationType = _generationType;
    check = _check;
    journalEntry = _journalEntry;
}

bool Requirement::checkFullfilled(Character* c)
{
    isFullfilled = check(c);
    return isFullfilled;
}

void Requirement::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << isFullfilled;
    packet << sf::Uint32(journalEntry);
}