#include "JournalEntry.hpp"
#include <string>

std::string JournalEntry::getTitle(Entry e)
{
    switch(e)
    {
        default:
        case Entry::None:
            return "None";
        case Entry::TestMissionRoot:
            return "Mission Root Test";
        case Entry::FindLevelTwo:
            return "Find Level Two";
        case Entry::GetDoubleHealth:
            return "Get Double Health";
    }
}

std::string JournalEntry::getDescription(Entry e)
{
    switch(e)
    {
        default:
        case Entry::None:
            return "None.";
        case Entry::TestMissionRoot:
            return "Mission Root Test";
        case Entry::FindLevelTwo:
            return "Find Level Two";
        case Entry::GetDoubleHealth:
            return "Get Double Health";
    }
}