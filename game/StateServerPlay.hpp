#pragma once
#ifndef RZ_SERVERONLYSTATE_H
#define RZ_SERVERONLYSTATE_H

#include "../bitengine/Network.hpp"

class RogueZombieServer;

class StateServerPlay : public bit::ClientServerState
{
public:

    StateServerPlay(bit::StateStack &stack, RogueZombieServer* game);

    virtual ~StateServerPlay();

    RogueZombieServer* rogueZombieServer;

    virtual void load();

    virtual bool update(sf::Time &gameTime);

protected:

    virtual bit::Server* newServer();

    virtual std::string getServerIpAddress();

    virtual unsigned short getServerPort();

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

    virtual void preparePacket_ClientDisconnect(bit::ClientPacket &packet);
};

#endif