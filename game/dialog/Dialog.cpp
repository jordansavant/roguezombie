#include "Dialog.hpp"

Dialog::Dialog()
    : id(0), entry(DialogEntry::Entry::None)
{
}

Dialog::Dialog(unsigned int id, DialogEntry::Entry entry)
    : id(id), entry(entry)
{
}