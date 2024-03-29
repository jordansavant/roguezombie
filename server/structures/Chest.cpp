#include "Chest.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Items/Item.hpp"
#include "../Structure.hpp"
#include "../Interaction.hpp"
#include "../Character.hpp"
#include "../ServerEvent.hpp"
#include <functional>

Chest::Chest()
    : Structure(),  schema()
{
    isDestroyable = true;
}

void Chest::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Chest, _x, _y, _level->tileWidth, _level->tileHeight);

    blockFoV = false;
    schema.isLocked = true;
    Structure::schema.interactable = true;
    Body::schema.obstructionRatio = .1;
}

void Chest::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}

void Chest::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    Structure::handleInteraction(interaction, interactor, responsePacket);

    switch(interaction)
    {
        case Interaction::Type::UnlockWithLockpick:
        case Interaction::Type::UnlockWithBash:
            // disabled until we want these mechanics in the game
            break;
        case Interaction::Type::UnlockWithKey:
        {
            bool wasLocked = false;

            // Check if the character has a keycard
            if (interactor && interactor->schema.type == Body::Type::Character)
            {
                ::Character* character = static_cast<::Character*>(interactor);
				if (character->hasAccess(schema.accessLevel))
				{
                    schema.isLocked = false;
                    wasLocked = true;
                }
            }

            // Success
            responsePacket << wasLocked;

            break;
        }
        case Interaction::Type::LockWithKey:
        case Interaction::Type::LockWithLockpick:
        {
            // TODO: Disable lock until we have a reason to lock
            schema.isLocked = true;

            // Success
            responsePacket << true;

            break;
        }
    }
}

void Chest::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Structure::prepareSnapshot(packet, full);

    packet << schema;
}

void Chest::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    if(schema.isLocked)
    {
        fill.push_back(Interaction::Type::UnlockWithKey);
        //fill.push_back(Interaction::Type::UnlockWithLockpick);
        //fill.push_back(Interaction::Type::UnlockWithBash);
    }
    else
    {
        fill.push_back(Interaction::Type::OpenInventory);
        //fill.push_back(Interaction::Type::LockWithKey);
        //fill.push_back(Interaction::Type::LockWithLockpick);
    }

    // Put the default ones after our primary ones
    Structure::getAvailableInteractions(fill);
}

void Chest::onInventoryOpen(Body* guest)
{
    if(guest && guest->schema.type == Body::Type::Character)
    {
        ::Character* guestCharacter = static_cast<::Character*>(guest);
        if(guestCharacter->schema.isPlayerCharacter && guestCharacter->schema.player)
        {
            Player* player = guestCharacter->schema.player;
            level->sendEventToPlayer(player, [] (bit::ServerPacket &packet) -> void {
                packet << sf::Uint32(ServerEvent::ChestOpen);
            });
        }
    }
}

void Chest::onInventoryClose(Body* guest)
{
    if(guest && guest->schema.type == Body::Type::Character)
    {
        ::Character* guestCharacter = static_cast<::Character*>(guest);
        if(guestCharacter->schema.isPlayerCharacter && guestCharacter->schema.player)
        {
            Player* player = guestCharacter->schema.player;
            level->sendEventToPlayer(player, [] (bit::ServerPacket &packet) -> void {
                packet << sf::Uint32(ServerEvent::ChestClose);
            });
        }
    }
}