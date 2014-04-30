#pragma once
#ifndef RZ_INTERACTION_H
#define RZ_INTERACTION_H

#include <string>

class Interaction
{
public:
    enum Type
    {
        LockableWithKey,
        LockableWithLockpick,
        UnlockableWithKey,
        UnlockableWithLockpick,
        UnlockableWithBash,
    };

    static std::string getTitle(Type type);
};

#endif