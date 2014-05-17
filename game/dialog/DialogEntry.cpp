#include "DialogEntry.hpp"
#include "DialogNode.hpp"
#include "Dialog.hpp"

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

unsigned int DialogEntry::dialogNodeId = 0;

DialogNode* DialogEntry::getDialogTree()
{
    DialogNode* e = new DialogNode;
    e->prompt = Dialog(++dialogNodeId, Entry::Speaker_Bye);
    
    DialogNode* d = new DialogNode;
    d->prompt = Dialog(++dialogNodeId, Entry::Speaker_ThatsUnfortunate);
    d->addResponse(Dialog(++dialogNodeId, Entry::Listener_Yes), e);
    
    DialogNode* c = new DialogNode;
    c->prompt = Dialog(++dialogNodeId, Entry::Speaker_ThatsFortunate);
    c->addResponse(Dialog(++dialogNodeId, Entry::Listener_Yes), e);
    
    DialogNode* b = new DialogNode;
    b->prompt = Dialog(++dialogNodeId, Entry::Speaker_HowAreYou);
    b->addResponse(Dialog(++dialogNodeId, Entry::Listener_IAmGood), c);
    b->addResponse(Dialog(++dialogNodeId, Entry::Listener_IAmBad), d);
    
    DialogNode* a = new DialogNode;
    a->prompt = Dialog(++dialogNodeId, Entry::Speaker_Hello);
    a->addResponse(Dialog(++dialogNodeId, Entry::Listener_Hello), b);
    
    return a;
}