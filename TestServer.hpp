#pragma once
#ifndef RZ_TESTSERVER_H
#define RZ_TESTSERVER_H

#include "bitengine/Game/Server.hpp"
#include "SFML/Network.hpp"

namespace bit
{
    class RemoteClient;
}

class TestServer : public bit::Server
{
private:

    virtual void handlePacket_PeerClientEvent(sf::Packet &packet, bit::RemoteClient &client);

    virtual void handlePacket_PeerClientRealtimeChange(sf::Packet &packet, bit::RemoteClient &client);

    virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

    virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerClientConnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerClientDisconnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
};

#endif