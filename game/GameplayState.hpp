#pragma once
#ifndef RZ_GAMEPLAYSTATE_H
#define RZ_GAMEPLAYSTATE_H

#include "../bitengine/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "LevelClient.hpp"
#include "Command.hpp"

class GameplayState : public bit::ClientServerState
{
public:

    GameplayState(bit::StateStack &stack, bit::Game* game, bool isHost);

    virtual ~GameplayState();

    LevelClient* levelClient;
	std::vector<Command> commandQueue;
    sf::Vector2f mousePositionInLevel;
    bit::FrameTimer fps;

    virtual void load();

	void now();

    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

protected:

    virtual bit::Server* newServer();

    virtual std::string getServerIpAddress();

    // Packet Handling

    virtual void handlePacket_Broadcast(bit::ServerPacket &packet);

    virtual void handlePacket_InitializeSelf(bit::ServerPacket &packet);

    virtual void handlePacket_InitializeWorld(bit::ServerPacket &packet);

    virtual void handlePacket_PeerClientConnected(bit::ServerPacket &packet);

    virtual void handlePacket_ClientDisonnected(bit::ServerPacket &packet);

    virtual void handlePacket_ServerUpdate(bit::ServerPacket &packet);

    virtual void handlePacket_Shutdown(bit::ServerPacket &packet);

    // Packet Sending

    virtual void preparePacket_ClientInformation(bit::ClientPacket &packet);

    virtual void preparePacket_ClientUpdate(bit::ClientPacket &packet);
};

#endif