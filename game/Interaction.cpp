#include "Interaction.hpp"
#include <string>

std::string Interaction::getTitle(Type type)
{
    switch(type)
    {
        default:
            return "";
        case Interaction::Type::LockableWithKey:
            return "Lock with key";
        case Interaction::Type::LockableWithLockpick:
            return "Lock with lockpick";
        case Interaction::Type::UnlockableWithKey:
            return "Unlock with key";
        case Interaction::Type::UnlockableWithLockpick:
            return "Unlock with lockpick";
        case Interaction::Type::UnlockableWithBash:
            return "Unlock with bash";
    }
}