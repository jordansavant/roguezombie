#include "Structure.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "ServerEvent.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Character.hpp"

Structure::Structure()
    : Body(), schema(), isDestroyable(true)
{
}

void Structure::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    Body::load(_level, _id, Body::Type::Structure, _x, _y, _width, _height);
    schema.type = _type;
}

void Structure::update(sf::Time &gameTime)
{
    Body::update(gameTime);
}

void Structure::setPosition(float x, float y)
{
    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::schema.x, Body::schema.y, Body::schema.width, Body::schema.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        currentTiles[i]->setOccupyingBody(this);
    }

    Body::schema.x = x;
    Body::schema.y = y;
}

void Structure::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    Body::getAvailableInteractions(fill);

    if (isDestroyable)
    {
        // Can destroy structures
        fill.push_back(Interaction::Type::Destroy);
    }
}

void Structure::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    Body::handleInteraction(interaction, interactor, responsePacket);

    switch (interaction)
    {
        case Interaction::Type::Destroy:
        {
            // I can destroy certain structures
            level->removeStructure(this); // THIS Maybe should be queued if it causes trouble inside of loops or ee

            // Send e
            if (interactor && interactor->schema.type == Body::Type::Character)
            {
                ::Character* character = static_cast<::Character*>(interactor);
                Player* player = character->schema.player;
                if (player) {
                    level->sendEventToPlayer(player, [](bit::ServerPacket &packet) -> void {
                        packet << sf::Uint32(ServerEvent::StructureDestroyed);
                        // TODO: Could Send the Structure::Type here so we could customize the client side effects
                    });
                }
            }

            // Success
            responsePacket << true;

            break;
        }
    }
}

void Structure::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::prepareSnapshot(packet, full);

    packet << schema;
}