#pragma once
#ifndef RZ_MULTIPLAYER_STATE_H
#define RZ_MULTIPLAYER_STATE_H

#include "../bitengine/Game/ClientServerState.hpp"
#include "ClientWorld.hpp"

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

    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);


private:

    virtual bit::Server* newServer();

    // Packet Handling

    virtual void handlePacket_Broadcast(sf::Packet &packet);

    virtual void handlePacket_InitializeSelf(sf::Packet &packet);

    virtual void handlePacket_PeerClientConnected(sf::Packet &packet); 

    virtual void handlePacket_ClientDisonnected(sf::Packet &packet);

    virtual void handlePacket_ServerUpdate(sf::Packet &packet);

    virtual void handlePacket_Shutdown(sf::Packet &packet);

    // Packet Sending

    virtual sf::Packet& preparePacket_ClientInformation(sf::Packet &packet);

    virtual sf::Packet& preparePacket_ClientUpdate(sf::Packet &packet);

};


#endif