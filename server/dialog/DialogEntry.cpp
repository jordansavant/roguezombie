#include "DialogEntry.hpp"
#include "DialogNode.hpp"
#include "Dialog.hpp"
#include "../Body.hpp"
#include "../items/Item.hpp"

std::string DialogEntry::getString(DialogEntry::Entry entry)
{
    switch(entry)
    {
        default:
        case DialogEntry::Entry::None:
            return "";
        case DialogEntry::Entry::Speaker_Hello:
            return "Greetings and welcome to the abyssal\nfuture where your kind eats my kind.";
        case DialogEntry::Entry::Listener_Hello:
            return "Thank you and greetings as well.";
        case DialogEntry::Entry::Speaker_HowAreYou:
            return "How are you?";
        case DialogEntry::Entry::Listener_IAmGood:
            return "Much brains, much good!";
        case DialogEntry::Entry::Listener_IAmBad:
            return "No brains, no good!";
        case DialogEntry::Entry::Speaker_ThatsFortunate:
            return "That's good to hear.";
        case DialogEntry::Entry::Speaker_ThatsUnfortunate:
            return "I'm sorry to hear that.";
        case DialogEntry::Entry::Listener_Yes:
            return "Yar.";
        case DialogEntry::Entry::Speaker_Bye:
            return "Well, later then.";
    }
}