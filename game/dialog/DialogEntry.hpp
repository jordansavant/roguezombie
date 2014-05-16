#pragma once
#ifndef RZ_DIALOGENTRY_H
#define RZ_DIALOGENTRY_H

#include <string>

class DialogEntry
{
public:

    enum Entry
    {
        None,
        Speaker_Hello,
        Listener_Hello,
        Speaker_HowAreYou,
        Listener_IAmGood,
        Listener_IAmBad,
        Speaker_ThatsFortunate,
        Speaker_ThatsUnfortunate,
        Listener_Yes,
        Speaker_Bye,
    };

    static std::string getString(Entry entry);
};

#endif