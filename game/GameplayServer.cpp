#include "GameplayServer.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/System.hpp"
#include "Command.hpp"
#include "Player.hpp"

GameplayServer::GameplayServer()
    : bit::Server()
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
        levels[i].load(i);
    }
}

void GameplayServer::update(sf::Time &gameTime)
{
    for(unsigned int i=0; i < levels.size(); i++)
    {
        levels[i].update(gameTime);
    }
}

// Packet Handling

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
                        levels[0].removePlayer(player);
                        levels[1].createPlayer(player);
                        break;
                    case 1:
                        levels[1].removePlayer(player);
                        levels[0].createPlayer(player);
                        break;
                }
                break;
		}
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