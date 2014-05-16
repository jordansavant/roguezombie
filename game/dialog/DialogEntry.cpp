#include "DialogEntry.hpp"

std::string DialogEntry::getString(DialogEntry::Entry entry)
{
    switch(entry)
    {
        default:
        case DialogEntry::Entry::None:
            return "";
        case DialogEntry::Entry::Speaker_Hello:
            return "Greetings.";
        case DialogEntry::Entry::Listener_Hello:
            return "Hello.";
        case DialogEntry::Entry::Speaker_HowAreYou:
            return "How are you?";
        case DialogEntry::Entry::Listener_IAmGood:
            return "Doing well.";
        case DialogEntry::Entry::Listener_IAmBad:
            return "Doing poorly.";
        case DialogEntry::Entry::Speaker_ThatsFortunate:
            return "That's good to hear.";
        case DialogEntry::Entry::Speaker_ThatsUnfortunate:
            return "That's no good.";
        case DialogEntry::Entry::Listener_Yes:
            return "Yeah.";
        case DialogEntry::Entry::Speaker_Bye:
            return "Later.";
    }
}