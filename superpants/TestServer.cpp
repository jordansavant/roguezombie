#include "TestServer.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/System.hpp"
#include "Command.hpp"

void TestServer::load()
{
    world.load();
}

void TestServer::update(sf::Time &gameTime)
{
    world.update(gameTime);
}

// Packet Handling

void TestServer::handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	world.createPlayer(client);
}

void TestServer::handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client update");

	// Get command count
	sf::Uint32 commandCount;
	packet >> commandCount;

	for(unsigned int i=0; i < commandCount; i++)
	{
		// Get command
		sf::Int32 commandType;
		packet >> commandType;

		// Determine how to handle
		switch(commandType)
		{
			case Command::Type::PlayerMoveUp:
			case Command::Type::PlayerMoveDown:
			case Command::Type::PlayerMoveLeft:
			case Command::Type::PlayerMoveRight:
            case Command::Type::PlayerTeleport:
				world.handlePlayerCommand(packet, client, static_cast<Command::Type>(commandType));
				break;
		}
	}
}

/**
 * Prepare Outgoing Server Packets
 **/

void TestServer::preparePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize self");

    world.prepareSnapshot(packet, true);
}

void TestServer::preparePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
}

void TestServer::preparePacket_PeerClientDisconnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
}

void TestServer::preparePacket_ServerUpdate(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare server update");

    world.prepareSnapshot(packet);
}