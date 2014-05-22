#include "Player.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Character.hpp"

Player::Player()
    : level(NULL), character(NULL), clientId(0), client(NULL), requestFullSnapshot(false)
{
}

void Player::load(bit::RemoteClient* _client)
{
    client = _client;
    clientId = _client->id;
}

void Player::setLevel(Level* level)
{
    this->level = level;
}

void Player::setCharacter(Character* character)
{
    this->character = character;
}

void Player::handleCommand(bit::ClientPacket &packet, Command::Type commandType)
{
	sf::Vector2f d(0, 0);

	switch(commandType)
	{
        // Debug Commands
		case Command::Type::PlayerMoveUp:
            character->moveUp();
			break;
		case Command::Type::PlayerMoveDown:
            character->moveDown();
			break;
		case Command::Type::PlayerMoveLeft:
            character->moveLeft();
			break;
		case Command::Type::PlayerMoveRight:
            character->moveRight();
			break;

        // Free Mode Commands
        case Command::Type::Free_MoveToTile:
        {
            unsigned int tileId;
            packet >> tileId;

            if(level->state == Level::State::Free)
            {
                Tile* t = level->tiles[tileId];
                if(t)
                {
                    character->pathToPosition(t->schema.x, t->schema.y);
                }
            }
            break;
        }

        // Combat Commands
        case Command::Type::Combat_MoveToTile:
        {
            unsigned int tileId;
            packet >> tileId;

            // If we are in combat
            if(level->state == Level::State::Combat)
            {
                // If it is my turn
                if(character->combatState == Character::CombatState::DecideAction)
                {
                    Tile* t = level->tiles[tileId];
                    if(t)
                    {
                        character->combat_DecideAction_MoveToLocation(t->schema.x, t->schema.y);
                    }
                }
            }
            break;
        }
	}
}