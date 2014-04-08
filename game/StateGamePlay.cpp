#include "StateGamePlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "GameplayServer.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "Command.hpp"
#include "characters/ZombieClient.hpp"
#include <sstream>

StateGamePlay::StateGamePlay(bit::StateStack &stack, RogueZombieGame* _game, bool isClient, bool isHost)
    : bit::ClientServerState(stack, _game, isClient, isHost), rogueZombieGame(_game), fps()
{
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);
    createCamera(rogueZombieGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 3;
    levelClient = new LevelClient();
}

StateGamePlay::~StateGamePlay()
{
    delete levelClient;
}

void StateGamePlay::load()
{
    bit::ClientServerState::load();

    levelClient->load(this);
}

bool StateGamePlay::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);

    fps.update(gameTime);

    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;

	// Listen for Game Commands
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::X))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerSwitchLevel;
		commandQueue.push_back(cmd);
	}
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::W))
	{
        Command cmd;
        cmd.type = Command::Type::PlayerMoveUp;
		commandQueue.push_back(cmd);
	}
	if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::S))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveDown;
		commandQueue.push_back(cmd);
	}
	if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::A))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveLeft;
		commandQueue.push_back(cmd);
	}
	if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::D))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveRight;
		commandQueue.push_back(cmd);
	}
    if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
    {
        // See if a tile is being hovered over
        if(levelClient->hoveredTile)
        {
            TileClient* t = levelClient->hoveredTile;
            Command cmd;
            cmd.type = Command::Type::PlayerClickTile;
            cmd.pack = [t] (sf::Packet &packet) {
                packet << sf::Uint32(t->fixedState.id);
            };
		    commandQueue.push_back(cmd);
        }
    }
    if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Right))
    {
        // See if a tile is being hovered over
        if(levelClient->hoveredTile)
        {
            TileClient* t = levelClient->hoveredTile;
            Command cmd;
            cmd.type = Command::Type::PlayerRightClickTile;
            cmd.pack = [t] (sf::Packet &packet) {
                packet << sf::Uint32(t->fixedState.id);
            };
		    commandQueue.push_back(cmd);
        }
    }

	// Exit
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        disconnect();
    }

    levelClient->update(gameTime);
    
    levelClient->minimap.setPosition(150 * rogueZombieGame->currentResolutionRatioX, 50 * rogueZombieGame->currentResolutionRatioY);
    levelClient->minimap.setScale(rogueZombieGame->currentResolutionRatio, rogueZombieGame->currentResolutionRatio);

    // Camera
    if(levelClient->playerCharacter)
    {
        float toleranceX = 250 * rogueZombieGame->currentResolutionRatioX;
        float toleranceY = 150 * rogueZombieGame->currentResolutionRatioY;

        sf::Vector2f position(levelClient->playerCharacter->Body::deltaState.x, levelClient->playerCharacter->Body::deltaState.y);
        position = bit::VectorMath::normalToIsometric(position.x, position.y);
        cameras[0]->lock(position.x, position.y, toleranceX, toleranceY, 4.0, 100.0);
    }

    return true;
}

void StateGamePlay::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::draw(window, gameTime);
    
    window.draw(levelClient->minimap);

    fps.draw(window, gameTime);
}

void StateGamePlay::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    window.draw(*levelClient);
}

bit::Server* StateGamePlay::newServer()
{
    return new GameplayServer();
}

std::string StateGamePlay::getServerIpAddress()
{
    return "192.168.0.100";
}

unsigned short StateGamePlay::getServerPort()
{
    return BIT_SERVER_PORT;
}

/**
 * Packet handling
 */

void StateGamePlay::handlePacket_Broadcast(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void StateGamePlay::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void StateGamePlay::handlePacket_InitializeWorld(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize world");

    sf::Uint32 levelId;
    packet >> levelId;
    levelClient->levelId = levelId;
    levelClient->load(this);
    levelClient->handleSnapshot(packet, true);
}

void StateGamePlay::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client connected");
}

void StateGamePlay::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected");
}

void StateGamePlay::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
    //bit::Output::Debug("Client handle server update");

    sf::Uint32 levelId;
    packet >> levelId;
    if(levelClient->levelId != levelId)
    {
        bit::Output::Debug(levelId);
        delete levelClient;
        levelClient = new LevelClient();
        levelClient->levelId = levelId;
        levelClient->load(this);
    }

    levelClient->handleSnapshot(packet, true);
}

void StateGamePlay::handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected acknowledged");

    // Exit
    requestStackPop();
}

void StateGamePlay::handlePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}

/**
 * Packet sending
 */

void StateGamePlay::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client information");
}

void StateGamePlay::preparePacket_ClientUpdate(bit::ClientPacket &packet)
{
    //bit::Output::Debug("Client prepare client update");

	// Notify of command count
	sf::Uint32 commandCount;
	commandCount = commandQueue.size();
	packet << commandCount;

	for(unsigned int i=0; i < commandQueue.size(); i++)
	{
		// Push commands onto packet, oldest first
        packet << static_cast<sf::Uint8>(commandQueue[i].type);
        commandQueue[i].pack(packet);
	}

	// Clear commands
	commandQueue.clear();
}

void StateGamePlay::preparePacket_ClientDisconnect(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client disconnect");
}
