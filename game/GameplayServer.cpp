#include "GameplayServer.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/System.hpp"
#include "Command.hpp"
#include "Player.hpp"
#include "levels/Interior.hpp"

GameplayServer::GameplayServer()
    : bit::Server(), tileIdCounter(0), bodyIdCounter(0)
{
}

GameplayServer::~GameplayServer()
{
    for(auto iterator = players.begin(); iterator != players.end(); iterator++)
    {
        delete iterator->second;
    }
}

void GameplayServer::load()
{
    levels.resize(2);
    for(unsigned int i=0; i < levels.size(); i++)
    {
        switch(i)
        {
            case 0:
            {
                levels[i].load(this, i, Interior::level_01, Interior::level_01_rows, Interior::level_01_cols);
                break;
            }
            case 1:
            {
                levels[i].load(this, i, Interior::level_02, Interior::level_02_rows, Interior::level_02_cols);
                break;
            }
        }
    }
}

void GameplayServer::update(sf::Time &gameTime)
{
    // Serve requests
    for(unsigned int i=0; i < pendingMoves.size(); i++)
    {
        levels[pendingMoves[i].fromLevelId].removePlayer(pendingMoves[i].player);
        levels[pendingMoves[i].toLevelId].addPlayer(pendingMoves[i].player);
    }
    pendingMoves.clear();

    for(unsigned int i=0; i < levels.size(); i++)
    {
        levels[i].update(gameTime);
    }
}

unsigned int GameplayServer::getNextBodyId()
{
    return ++bodyIdCounter;
}

void GameplayServer::movePlayerToLevel(Player* player, unsigned int fromLevelId, unsigned int toLevelId)
{
    PendingMovePlayer m;
    m.player = player;
    m.fromLevelId = fromLevelId;
    m.toLevelId = toLevelId;
    pendingMoves.push_back(m);
}


/**
 * Handle Incoming Client Packets
 **/

void GameplayServer::handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	if(players.find(client.id) == players.end())
    {
        Player* p = new Player();
        p->load(client.id);
        players[client.id] = p;

	    levels[0].createPlayer(p);
    }
}

void GameplayServer::handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    //bit::Output::Debug("Server handle client update");
    Player* player = players[client.id];

	// Get command count
	sf::Uint32 commandCount;
	packet >> commandCount;

	for(unsigned int i=0; i < commandCount; i++)
	{
		// Get command
		sf::Uint8 commandType;
		packet >> commandType;

		// Determine how to handle
		switch(commandType)
		{
			case Command::Type::PlayerMoveUp:
			case Command::Type::PlayerMoveDown:
			case Command::Type::PlayerMoveLeft:
			case Command::Type::PlayerMoveRight:
            case Command::Type::PlayerClickTile:
            case Command::Type::PlayerRightClickTile:
                player->level->handlePlayerCommand(packet, client, static_cast<Command::Type>(commandType));
				break;
            case Command::Type::PlayerSwitchLevel:
                switch(player->level->id)
                {
                    case 0:
                        movePlayerToLevel(player, 0, 1);
                        break;
                    case 1:
                        movePlayerToLevel(player, 1, 0);
                        break;
                }
                break;
		}
	}
}

void GameplayServer::handlePacket_ClientDisconnect(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client disconnect");

    // Remove them from their last level
    auto itr = players.find(client.id);
    if(itr != players.end())
    {
        Player* p = itr->second;
        players.erase(client.id);
        p->level->deletePlayer(p);
    }
}


void GameplayServer::handle_ClientTimeout(bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client timeout");

    // Remove them from their last level
    auto itr = players.find(client.id);
    if(itr != players.end())
    {
        Player* p = itr->second;
        players.erase(client.id);
        p->level->deletePlayer(p);
    }
}



/**
 * Prepare Outgoing Server Packets
 **/

void GameplayServer::preparePacket_InitializeSelf(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare initialize self");
}

void GameplayServer::preparePacket_InitializeWorld(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare initialize level");

    Player* player = players[client.id];
    packet << sf::Uint32(player->level->id);
    player->level->prepareSnapshot(packet, client, true);
}

void GameplayServer::preparePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
}

void GameplayServer::preparePacket_PeerClientDisconnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
}

void GameplayServer::preparePacket_ServerUpdate(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    //bit::Output::Debug("Server prepare server update");

    Player* player = players[client.id];
    packet << sf::Uint32(player->level->id);
    player->level->prepareSnapshot(packet, client, true);
}

void GameplayServer::preparePacket_DisconnectAcknowledge(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare server disconnect acknowledge");
}

void GameplayServer::preparePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare server shutdown");
}