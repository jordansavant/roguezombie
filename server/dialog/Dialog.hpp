#pragma once
#ifndef RZ_DIALOG_H
#define RZ_DIALOG_H

#include "DialogEntry.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Network.hpp"
#include <string>

class Dialog
{
public:

    Dialog();

    Dialog(unsigned int id, DialogEntry::Entry entry);

    unsigned int id;
    DialogEntry::Entry entry;

    friend sf::Packet& operator <<(sf::Packet& packet, const Dialog &dialog)
    {
        packet << sf::Uint32(dialog.id);
        packet << sf::Uint32(dialog.entry);
        return packet;
    }

    friend sf::Packet& operator >>(sf::Packet& packet, Dialog &dialog)
    {
        packet >> dialog.id;
        bit::NetworkHelper::unpackEnum<sf::Uint32, DialogEntry::Entry>(packet, dialog.entry);
        return packet;
    }

};

#endif