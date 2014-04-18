#pragma once
#ifndef RZ_JOURNALENTRY_H
#define RZ_JOURNALENTRY_H

#include <string>

class JournalEntry
{
public:

    enum Entry
    {
        None,
        FindLevelTwo
    };

    JournalEntry(std::string title, std::string description);

    JournalEntry(Entry e);

    std::string title;
    std::string description;

    static JournalEntry get(Entry e);

};

#endif