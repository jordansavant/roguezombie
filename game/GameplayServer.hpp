#pragma once
#ifndef RZ_GAMEPLAYSERVER_H
#define RZ_GAMEPLAYSERVER_H

#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "World.hpp"

class GameplayServer : public bit::Server
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