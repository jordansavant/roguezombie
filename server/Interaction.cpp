#include "Interaction.hpp"
#include <string>

std::string Interaction::getTitle(Type type)
{
    switch(type)
    {
        default:
            return "(Missing Interaction Label)";
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
        case Interaction::Type::OpenInventory:
            return "Loot";
        case Interaction::Type::CloseInventory:
            return "Close";
        case Interaction::Type::Dialog:
            return "Talk";
        case Interaction::Type::Consume:
            return "Consume";
    }
}