#include "RequirementClient.hpp"
#include "../../bitengine/System/Output.hpp"

RequirementClient::RequirementClient()
    : id(0), isFullfilled(false), journalEntry("", "")
{
}

void RequirementClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> isFullfilled;
    unsigned int journalEntryInt;
    packet >> journalEntryInt;

    journalEntry = JournalEntry::get(static_cast<JournalEntry::Entry>(journalEntryInt));
    bit::Output::Debug(journalEntry.title);
}