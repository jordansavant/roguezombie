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

		unsigned int id;
        sf::TcpSocket socket;
        sf::Time lastPacketTime;
        bool isConnected;
        bool isConfirmed;
        bool hasTimedOut;
        sf::Uint32 lastAcknowledgedSnapshotId;
    };
}

#endif