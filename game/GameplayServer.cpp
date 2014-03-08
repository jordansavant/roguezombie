#include "GameplayServer.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/System.hpp"
#include "Command.hpp"

void GameplayServer::load()
{
    world.load();
}

void GameplayServer::update(sf::Time &gameTime)
{
    world.update(gameTime);
}

// Packet Handling

void GameplayServer::handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	world.createPlayer(client);
}

void GameplayServer::handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    //bit::Output::Debug("Server handle client update");

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
				world.handlePlayerCommand(packet, client, static_cast<Command::Type>(commandType));
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
    bit::Output::Debug("Server prepare initialize world");

    world.prepareSnapshot(packet, client, true);
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

    world.prepareSnapshot(packet, client, true);
}