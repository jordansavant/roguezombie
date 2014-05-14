#pragma once
#ifndef RZ_INTERACTION_H
#define RZ_INTERACTION_H

#include <string>

class Interaction
{
public:
    enum Type
    {
        LockWithKey,
        LockWithLockpick,
        UnlockWithKey,
        UnlockWithLockpick,
        UnlockWithBash,
        OpenInventory,
        CloseInventory,
        Converse,
    };

    static std::string getTitle(Type type);

};

#endif