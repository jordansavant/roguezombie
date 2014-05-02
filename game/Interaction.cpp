#include "Interaction.hpp"
#include <string>

std::string Interaction::getTitle(Type type)
{
    switch(type)
    {
        default:
            return "";
        case Interaction::Type::LockWithKey:
            return "Lock with key";
        case Interaction::Type::LockWithLockpick:
            return "Lock with lockpick";
        case Interaction::Type::UnlockWithKey:
            return "Unlock with key";
        case Interaction::Type::UnlockWithLockpick:
            return "Unlock with lockpick";
        case Interaction::Type::UnlockWithBash:
            return "Unlock with bash";
    }
}