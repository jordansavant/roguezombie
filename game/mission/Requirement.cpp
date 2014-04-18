#include "Requirement.hpp"
#include "../Character.hpp"
#include "../../bitengine/Network.hpp"

Requirement::Requirement()
    : id(0), check(NULL), isFullfilled(false), generationType(GenerationType::Scripted), journalEntry(JournalEntry::Entry::None)
{
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