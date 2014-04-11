#pragma once
#ifndef RZ_REQUIREMENT_H
#define RZ_REQUIREMENT_H

#include <functional>

class Character;

class Requirement
{
public:

    Requirement();

    std::function<bool(Character*)> check;
    bool isFullfilled;
    enum GenerationType
    {
        Scripted,
        Generated
    };
    GenerationType generationType;
    // unsigned int phoeneticVerbId; // if Generated
    // unsigned int phoeneticNounId; // if Generated
    // unsigned int journalId;       // if Scripted

    bool checkFullfilled(Character* c);
};

#endif