#pragma once
#ifndef RZ_MULTIPLAYER_STATE_H
#define RZ_MULTIPLAYER_STATE_H

#include "../bitengine/Game/ClientServerState.hpp"
#include "ClientWorld.hpp"
#include "Command.hpp"

namespace bit
{
    class StateStack;
    class Camera;
    class Game;
    class Server;
}

class MultiplayerState : public bit::ClientServerState
{
public:

    MultiplayerState(bit::StateStack &stack, bit::Game* game, bool isHost);
    
    ClientWorld clientWorld;

    virtual void load();

	void now();

    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

	std::vector<Command> commandQueue;

protected:

    virtual bit::Server* newServer();

    virtual std::string getServerIpAddress();

    // Packet Handling

    virtual void handlePacket_Broadcast(sf::Packet &packet);

    virtual void handlePacket_InitializeSelf(sf::Packet &packet);

    virtual void handlePacket_PeerClientConnected(sf::Packet &packet); 

    virtual void handlePacket_ClientDisonnected(sf::Packet &packet);

    virtual void handlePacket_ServerUpdate(sf::Packet &packet);

    virtual void handlePacket_Shutdown(sf::Packet &packet);

    // Packet Sending

    virtual void preparePacket_ClientInformation(sf::Packet &packet);

    virtual void preparePacket_ClientUpdate(sf::Packet &packet);

};


#endif