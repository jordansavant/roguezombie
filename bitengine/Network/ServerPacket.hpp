#pragma once
#ifndef BIT_SERVERPACKET_H
#define BIT_SERVERPACKET_H

#include "SFML/Network.hpp"

namespace bit
{
    class ServerPacket : public sf::Packet
    {
    public:

        ServerPacket();

        virtual const void* onSend(std::size_t& size);

        virtual void onReceive(const void* data, std::size_t size);
    };
}

#endif