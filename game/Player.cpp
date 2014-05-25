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
        case Command::Type::CombatCommand:
        {
            Command::CombatTarget target;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Command::CombatTarget>(packet, target);

            switch(target)
            {
                // Tile Targetted commands
                case Command::CombatTarget::Tile:
                {
                    unsigned int tileId;
                    packet >> tileId;
                    Command::TileTargetCommand cmd;
                    bit::NetworkHelper::unpackEnum<sf::Uint32, Command::TileTargetCommand>(packet, cmd);

                    switch(cmd)
                    {
                        case Command::TileTargetCommand::Move:
                        {
                            // Ensure we are in combat
                            if(validateCombat())
                            {
                                // Find the tile and issue decision to move to it
                                Tile* t = level->tiles[tileId];
                                if(t)
                                {
                                    character->combat_DecideAction_MoveToLocation(t->schema.x, t->schema.y);
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            break;
        }
	}
}

bool Player::validateCombat()
{
    return level->state == Level::State::Combat && character && character->combatState == Character::CombatState::DecideAction;
}