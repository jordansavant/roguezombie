#pragma once
#ifndef RZ_GAMEPLAYSERVER_H
#define RZ_GAMEPLAYSERVER_H

#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "Level.hpp"

class GameplayServer : public bit::Server
{
public:

    GameplayServer();

    virtual ~GameplayServer();

    virtual unsigned int getNextTileId();

    virtual unsigned int getNextBodyId();

    virtual void movePlayerToLevel(Player* player, unsigned int fromLevelId, unsigned int toLevelId);

private:

    struct PendingMovePlayer
    {
        Player* player;
        unsigned int fromLevelId;
	    unsigned int toLevelId;
    };
    std::vector<PendingMovePlayer> pendingMoves;

    Level level;
    std::vector<Level> levels;
	std::map<unsigned int, Player*> players;
    unsigned int tileIdCounter;
    unsigned int bodyIdCounter;

    virtual void load();

    virtual void update(sf::Time &gameTime);

    // Packet handling

    virtual void handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client);

    virtual void handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client);

    // Packet sending

    virtual void preparePacket_InitializeSelf(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_InitializeWorld(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_PeerClientConnected(bit::ServerPacket &packet);

    virtual void preparePacket_PeerClientDisconnected(bit::ServerPacket &packet);

    virtual void preparePacket_ServerUpdate(bit::ServerPacket &packet, bit::RemoteClient &client);
};

#endif