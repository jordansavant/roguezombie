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
        
        bool isNetworkConnected; // If the client is network connected
        bool isConfirmed; // If the client has sent setup data and received server world initialization
        bool hasTimedOut;
        bool hasDisconnected;
        bool hasBeenKicked;
        sf::Uint32 lastAcknowledgedSnapshotId;

        bool badConnection();
        bool canReceiveGamePackets();
    };
}

#endif