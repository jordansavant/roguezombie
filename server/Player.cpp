#include "Player.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "ServerEvent.hpp"
#include "GameplayServer.hpp"
#include "items/Item.hpp"
#include "Character.hpp"

Player::Player()
    : level(NULL), controlState(ControlState::Normal), character(NULL), spectatee(NULL), clientId(0), client(NULL), requestFullSnapshot(false)
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
    
    if(character)
    {
        character->level = level;
    }
}

// Used in player creation and player requickening
void Player::setCharacter(Character* character)
{
    this->character = character;

    // If I was spectating, do not
    if(spectatee)
    {
        unsetSpectatee();
    }

    // Register character
    character->onDeath += std::bind(&Player::onCharacterDeath, this, std::placeholders::_1);
    character->setControllingPlayer(this);
    controlState = ControlState::Normal;

}

void Player::unsetCharacter()
{
    character->unsetControllingPlayer();
    character = NULL;
}

void Player::setupPlayerCharacter()
{
    // Mission number 1
    //Mission* root = new Mission();
    //root->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::TestMissionRoot);
    //
    //Mission* healthMission = new Mission();
    //healthMission->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::GetDoubleHealth);
    //healthMission->assignRequirement([] (Character* c) -> bool {
    //    return (c->schema.health >= 200);
    //});
    //root->assignChildMission(healthMission);
    //
    //Mission* levelMission = new Mission();
    //levelMission->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::FindLevelTwo);
    //levelMission->assignRequirement([] (Character* c) -> bool {
    //    return (c->level == &c->level->server->levels[1]);
    //});
    //root->assignChildMission(levelMission);
    //
    //p->character->assignMission(root);

    // Items
    Item* magnum = Item::create(Item::Type::Magnum357, level->server->getNextItemId());
    character->addItemToInventory(magnum);

    Item* rifle = Item::create(Item::Type::Z4Rifle, level->server->getNextItemId());
    character->addItemToInventory(rifle);

    Item* crowbar = Item::create(Item::Type::Crowbar, level->server->getNextItemId());
    character->addItemToInventory(crowbar);

    Item* hardhat = Item::create(Item::Type::HardHat, level->server->getNextItemId());
    character->addItemToInventory(hardhat);

    character->equipFromInventory(Character::EquipmentSlot::WeaponPrimary, magnum->schema.id);
    character->equipFromInventory(Character::EquipmentSlot::WeaponSecondary, crowbar->schema.id);
    character->equipFromInventory(Character::EquipmentSlot::Head, hardhat->schema.id);
}

// When a player dies
//  Character needs to be dettached
//  Player needs to attach to character as a spectator
//  Event needs to be sent to change to spectate mode
//  * checkPlayersForDefeat()
void Player::onCharacterDeath(Character* e)
{
    // incoming character is also my character

    // Set up spectation
    setSpectatee(character);
    
    // Clear character control
    unsetCharacter();

    // Send server signal for spectate mode
    level->server->sendEventToClient(*client, [](bit::ServerPacket &packet){
        packet << sf::Uint32(ServerEvent::SpectateBegin);
    });

}

void Player::setSpectatee(Character* character)
{
    // Set up spectation
    spectatee = character;
    spectatee->setSpectatingPlayer(this);
    controlState = ControlState::Spectate;
}

// Used when disconnecting or returning to game
void Player::unsetSpectatee()
{
    // Remove self as spectator from character
    spectatee->unsetSpectatingPlayer(this);
    // Clear spectator
    spectatee = NULL;
}


void Player::spectateNext()
{
    // TODO: this is a total hack
    // Iterate all of the level's characters until I find my spectator
    int index = -1;
    Character* newSpectatee = NULL;
    for(unsigned int i=0; i < level->characters.size(); i++)
    {
        if(level->characters[i] == spectatee)
        {
            index = i;
            break;
        }

    }

    // If I found my current spectator
    if(index >= 0)
    {
        // TODO: switch to only spectate player characters
            // if(candidate->schema.isPlayerCharacter && candidate->schema.player != this)
        // Iterate from that character to the next looking for the next character
        for(unsigned int i=index + 1; i < level->characters.size(); i++)
        {
            Character* candidate = level->characters[i];
            newSpectatee = candidate;
            break;
        }
        // If not found going forward, start at beginning and go back
        if(!newSpectatee)
        {
            for(unsigned int i=0; i < index; i++)
            {
                Character* candidate = level->characters[i];
                newSpectatee = candidate;
                break;
            }
        }
    }

    if(newSpectatee)
    {
        unsetSpectatee();
        setSpectatee(newSpectatee);
    }
}

void Player::handleCommand(bit::ClientPacket &packet, Command::Type commandType)
{
    sf::Vector2f d(0, 0);

    switch(controlState)
    {
        // Controlling Character
        case ControlState::Normal:
        {
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
                                        character->combat_DecideAction_SwapWeapon();
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
            break;
        }

        // Spectator controls
        case ControlState::Spectate:
        {
            switch(commandType)
            {
                // Debug Commands
                case Command::Type::PlayerMoveUp:
                    // spectatee->moveUp();
                    break;
                case Command::Type::PlayerMoveDown:
                    // spectatee->moveDown();
                    break;
                case Command::Type::PlayerMoveLeft:
                    // spectatee->moveLeft();
                    spectateNext();
                    break;
                case Command::Type::PlayerMoveRight:
                    // spectatee->moveRight();
                    spectateNext();
                    break;
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