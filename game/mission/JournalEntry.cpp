#include "JournalEntry.hpp"
#include <string>

JournalEntry::JournalEntry(std::string _title, std::string _description)
    : title(_title), description(_description)
{
}


JournalEntry::JournalEntry(Entry e)
{
    JournalEntry j = get(e);
    title = j.title;
    description = j.description;
}

JournalEntry JournalEntry::get(Entry e)
{
    switch(e)
    {
        default:
        case Entry::None:
            return JournalEntry("None", "None.");
        case Entry::FindLevelTwo:
            return JournalEntry("Find Level Two", "Find your way to level two and see what lurks within.");
    }
}