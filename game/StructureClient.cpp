#include "StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"

StructureClient::StructureClient()
    : BodyClient(), schema()
{
}

void StructureClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    BodyClient::handleSnapshot(packet, full);

    packet >> schema;
}