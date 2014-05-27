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
        case Command::Type::FreeCommand:
        {
            Command::Target target;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Command::Target>(packet, target);

            switch(target)
            {
                // Tile Targetted commands
                case Command::Target::Tile:
                {
                    unsigned int tileId;
                    packet >> tileId;
                    Command::TargetTileCommand cmd;
                    bit::NetworkHelper::unpackEnum<sf::Uint32, Command::TargetTileCommand>(packet, cmd);

                    switch(cmd)
                    {
                        case Command::TargetTileCommand::Move:
                        {
                            // Ensure we are in free mode
                            if(validateFree())
                            {
                                // Find the tile and issue decision to move to it
                                Tile* t = level->tiles[tileId];
                                if(t)
                                {
                                    character->pathToPosition(t->schema.x, t->schema.y);
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
            }
        }

        // Combat Commands
        case Command::Type::CombatCommand:
        {
            Command::Target target;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Command::Target>(packet, target);

            switch(target)
            {
                // Commands without a target
                case Command::Target::NoTarget:
                {
                    Command::NonTargetCommand cmd;
                    bit::NetworkHelper::unpackEnum<sf::Uint32, Command::NonTargetCommand>(packet, cmd);
                    switch(cmd)
                    {
                        case Command::NonTargetCommand::Skip:
                        {
                            if(validateCombat())
                            {
                                character->combat_DecideAction_Skip();
                            }

                            break;
                        }
                        case Command::NonTargetCommand::SwapWeapon:
                        {
                            if(validateCombat())
                            {
                                character->swapWeapons();
                            }

                            break;
                        }
                    }

                    break;
                }

                // Tile Targetted commands
                case Command::Target::Tile:
                {
                    unsigned int tileId;
                    packet >> tileId;
                    Command::TargetTileCommand cmd;
                    bit::NetworkHelper::unpackEnum<sf::Uint32, Command::TargetTileCommand>(packet, cmd);

                    switch(cmd)
                    {
                        case Command::TargetTileCommand::Move:
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

                // Character Targetted commands
                case Command::Target::Character:
                {
                    unsigned int tileId;
                    packet >> tileId;
                    Command::TargetCharacterCommand cmd;
                    bit::NetworkHelper::unpackEnum<sf::Uint32, Command::TargetCharacterCommand>(packet, cmd);

                    switch(cmd)
                    {
                        case Command::TargetCharacterCommand::Attack:
                        {
                            // Ensure we are in combat
                            if(validateCombat())
                            {
                                // Find the tile and issue decision to move to it
                                Tile* t = level->tiles[tileId];
                                if(t && t->body && t->body->schema.type == Body::Type::Character)
                                {
                                    Character* target = static_cast<Character*>(t->body);
                                    character->combat_DecideAction_AttackCharacter(target);
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

bool Player::validateFree()
{
    return level->state == Level::State::Free && character;
}

bool Player::validateCombat()
{
    return level->state == Level::State::Combat && character && character->combatState == Character::CombatState::DecideAction;
}