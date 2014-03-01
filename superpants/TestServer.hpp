#pragma once
#ifndef RZ_TESTSERVER_H
#define RZ_TESTSERVER_H

#include "../bitengine/Game/Server.hpp"
#include "SFML/Network.hpp"
#include "World.hpp"

namespace bit
{
    class RemoteClient;
    class ServerPacket;
    class ClientPacket;
}

class TestServer : public bit::Server
{
private:

    World world;

    virtual void load();

    virtual void update(sf::Time &gameTime);

    // Packet handling

    virtual void handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client);

    virtual void handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client);

    // Packet sending

    virtual void preparePacket_InitializeSelf(bit::ServerPacket &packet);

    virtual void preparePacket_PeerClientConnected(bit::ServerPacket &packet);

    virtual void preparePacket_PeerClientDisconnected(bit::ServerPacket &packet);

    virtual void preparePacket_ServerUpdate(bit::ServerPacket &packet);
};

#endif