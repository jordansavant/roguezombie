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
    class ServerPacket;
    class ClientPacket;
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

    virtual void handlePacket_Broadcast(bit::ServerPacket &packet);

    virtual void handlePacket_InitializeSelf(bit::ServerPacket &packet);

    virtual void handlePacket_PeerClientConnected(bit::ServerPacket &packet); 

    virtual void handlePacket_ClientDisonnected(bit::ServerPacket &packet);

    virtual void handlePacket_ServerUpdate(bit::ServerPacket &packet);

    virtual void handlePacket_Shutdown(bit::ServerPacket &packet);

    // Packet Sending

    virtual void preparePacket_ClientInformation(bit::ClientPacket &packet);

    virtual void preparePacket_ClientUpdate(bit::ClientPacket &packet);

};


#endif