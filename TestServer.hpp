#pragma once
#ifndef RZ_TESTSERVER_H
#define RZ_TESTSERVER_H

#include "bitengine/Game/Server.hpp"
#include "SFML/Network.hpp"

class TestServer : public bit::Server
{
private:

    virtual void handlePacket_PlayerEvent(sf::Packet &packet, RemotePeer &peer);

    virtual void handlePacket_PlayerRealtimeChange(sf::Packet &packet, RemotePeer &peer);

    virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

    virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerConnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_PeerDisconnected(sf::Packet &packet);

    virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
};

#endif