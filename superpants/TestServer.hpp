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

    World serverWorld;

    virtual void load();

    virtual void update(sf::Time &gameTime);

    virtual void handleNewClient(bit::RemoteClient &client);

    virtual void handlePacket_ClientUpdate(sf::Packet &packet, bit::RemoteClient &client);

    virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

    virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerClientConnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerClientDisconnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
};

#endif