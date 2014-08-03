#include "StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../server/Level.hpp"
#include "../server/Body.hpp"
#include "../server/Tile.hpp"

StructureClient::StructureClient()
    : BodyClient(), schema()
{
}

void StructureClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    BodyClient::handleSnapshot(packet, full);

    packet >> schema;
}