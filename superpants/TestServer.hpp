#pragma once
#ifndef RZ_TESTSERVER_H
#define RZ_TESTSERVER_H

#include "../bitengine/Game/Server.hpp"
#include "SFML/Network.hpp"
#include "World.hpp"

namespace bit
{
    class RemoteClient;
}

class TestServer : public bit::Server
{
private:

    World world;

    virtual void load();

    virtual void update(sf::Time &gameTime);

    // Packet handling

    virtual void handlePacket_ClientInformation(sf::Packet &packet, bit::RemoteClient &client);

    virtual void handlePacket_ClientUpdate(sf::Packet &packet, bit::RemoteClient &client);

    // Packet sending

    virtual void preparePacket_InitializeSelf(sf::Packet &packet);

    virtual void preparePacket_PeerClientConnected(sf::Packet &packet);

    virtual void preparePacket_PeerClientDisconnected(sf::Packet &packet);

    virtual void preparePacket_ServerUpdate(sf::Packet &packet);
};

#endif