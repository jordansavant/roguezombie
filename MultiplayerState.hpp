#pragma once
#ifndef RZ_MULTIPLAYER_STATE_H
#define RZ_MULTIPLAYER_STATE_H

#include "bitengine/Game/ClientServerState.hpp"

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
    
    sf::Texture zombieimage;
    sf::Sprite zombiesprite;
	sf::CircleShape shape;
    
    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

private:
        virtual bit::Server* newServer();

        virtual void handlePacket_Broadcast(sf::Packet &packet);

        virtual void handlePacket_InitializeSelf(sf::Packet &packet);

        virtual void handlePacket_InitializeWorld(sf::Packet &packet);

        virtual void handlePacket_PeerConnected(sf::Packet &packet); 

        virtual void handlePacket_PeerDisonnected(sf::Packet &packet);

        virtual void handlePacket_Update(sf::Packet &packet);

        virtual void handlePacket_PeerEvent(sf::Packet &packet);

        virtual void handlePacket_PeerRealtimeChange(sf::Packet &packet);

        virtual void handlePacket_Shutdown(sf::Packet &packet);
};


#endif