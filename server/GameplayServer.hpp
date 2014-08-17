#pragma once
#ifndef RZ_GAMEPLAYSERVER_H
#define RZ_GAMEPLAYSERVER_H

#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"

class GameplayServer : public bit::Server
{
public:

    GameplayServer();

    virtual ~GameplayServer();

    enum KickReason
    {
        NoSpawn,
        CheatDetected,
    };

    virtual unsigned int getNextBodyId();

    virtual unsigned int getNextMissionId();

    virtual unsigned int getNextItemId();

    virtual unsigned int getNextDialogId();

    virtual void movePlayerToLevel(Player* player, unsigned int fromLevelId, unsigned int toLevelId);

private:

    struct PendingMovePlayer
    {
        Player* player;
        unsigned int fromLevelId;
	    unsigned int toLevelId;
    };
    std::vector<PendingMovePlayer> pendingMoves;

    LevelLoader levelLoader;
    std::vector<Level> levels;
    Level* startingLevel;
	std::map<unsigned int, Player*> players;
    unsigned int bodyIdCounter;
    unsigned int missionIdCounter;
    unsigned int itemIdCounter;
    unsigned int dialogIdCounter;

    virtual void load();

    virtual void update(sf::Time &gameTime);

    // Packet handling

    virtual void handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client);

    virtual void handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client);

    virtual void handlePacket_ClientDisconnect(bit::ClientPacket &packet, bit::RemoteClient &client);

    virtual void handle_ClientTimeout(bit::RemoteClient &client);

    virtual void handlePacket_ClientRequest(bit::ClientPacket &packet, bit::RemoteClient &client, bit::ServerPacket &responsePacket);

    // Packet sending

    virtual void preparePacket_InitializeSelf(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_InitializeWorld(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_PeerClientConnected(bit::ServerPacket &packet);

    virtual void preparePacket_PeerClientDisconnected(bit::ServerPacket &packet);

    virtual void preparePacket_ServerUpdate(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_DisconnectAcknowledge(bit::ServerPacket &packet, bit::RemoteClient &client);

    virtual void preparePacket_Shutdown(bit::ServerPacket &packet);
};

#endif