#pragma once
#ifndef RZ_STRUCTURECLIENT_H
#define RZ_STRUCTURECLIENT_H

#include "BodyClient.hpp"
#include "../server/Structure.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class StructureClient : public BodyClient
{
public:

	StructureClient();

	Structure::Schema schema;

	virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual std::string getTitle();
};

#endif