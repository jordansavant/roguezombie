#pragma once
#ifndef BIT_CLIENTPACKET_H
#define BIT_CLIENTPACKET_H

#include "SFML/Network.hpp"

namespace bit
{
    class ClientPacket : public sf::Packet
    {
    public:

        ClientPacket();

        virtual const void* onSend(std::size_t& size);

        virtual void onReceive(const void* data, std::size_t size);
    };
}

#endif