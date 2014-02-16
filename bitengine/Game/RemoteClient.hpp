#pragma once
#ifndef BIT_REMOTECLIENT_H
#define BIT_REMOTECLIENT_H

#include "SFML/Network.hpp"

namespace bit
{
    class RemoteClient
    {
    public:
        RemoteClient();

        sf::TcpSocket socket;
        sf::Time lastPacketTime;
        bool isReady;
        bool hasTimedOut;
    };
}

#endif