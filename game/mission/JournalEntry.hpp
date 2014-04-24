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
        TestMissionRoot,
        FindLevelTwo,
        GetDoubleHealth,
    };

    static std::string getTitle(Entry e);

    static std::string getDescription(Entry e);
};

#endif