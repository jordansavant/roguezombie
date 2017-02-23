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
    
    character->addItemToInventory(Item::create(Item::Type::Medkit, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::Brick, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::Grenade, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::Grenade, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::Grenade, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::Grenade, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::HardHat, level->server->getNextItemId()));
    character->addItemToInventory(Item::create(Item::Type::HardHat, level->server->getNextItemId()));
    
    character->equipFromInventory(Character::EquipmentSlot::WeaponPrimary, magnum->schema.id);
    character->equipFromInventory(Character::EquipmentSlot::WeaponSecondary, crowbar->schema.id);
    character->equipFromInventory(Character::EquipmentSlot::Head, hardhat->schema.id);

    character->schema.dexterity = 2; //TODO make me a better shot
    character->schema.maxHealth = 200;
    character->schema.health = 200;
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

    // Check for game over
    level->server->checkForGameOver();
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


void Player::spectateNext(int dir)
{
    bool forward = dir >= 0;

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
        // If I have another character on this level
        unsigned int nextIndex = index + 1;
        unsigned int prevIndex = index - 1;
        unsigned int targIndex = forward ? nextIndex : prevIndex;
        if(targIndex >= 0 && targIndex < level->characters.size())
        {
            // Get next character on this level
            newSpectatee = level->characters[targIndex];
            unsetSpectatee();
            setSpectatee(newSpectatee);
        }

        // If no more characters are on this level
        else
        {
            // Loop through next levels until
            Level* next = level;
            Character* candidateCharacter = NULL;
            while(candidateCharacter == NULL)
            {
                next = forward ? level->server->getNextLoadedLevel(next) : level->server->getPreviousLoadedLevel(next);

                // If I found a character on another level
                if(next->characters.size() > 0)
                {
                    unsigned int candidateIndex = forward ? 0 : next->characters.size() - 1;
                    candidateCharacter = next->characters[candidateIndex];

                    // If that character is my existing spectatee do nothing (full loop)
                    if(candidateCharacter == spectatee)
                        return;

                    // If that character is not on the same level as me
                    if(candidateCharacter->level != level)
                    {
                        unsetSpectatee();

                        // Transition to another level
                        level->players.erase(clientId);
                        setLevel(candidateCharacter->level);
                        level->players[clientId] = this;

                        setSpectatee(candidateCharacter);
                        return;
                    }
                    
                    // If that character is on my level
                    else
                    {
                        unsetSpectatee();
                        setSpectatee(candidateCharacter);
                        return;
                    }
                }
            }
        }
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

                // Item Mode Commands
                case Command::Type::ItemCommand:
                {
                    Item::Schema itemSchema;
                    packet >> itemSchema;

                    // Validate that we can issue the command
                    switch(itemSchema.commandType)
                    {
                        case Item::CommandType::CommandTypeSelf:
                        {
                            // Apply the item to myself if I am allowed given the current game mode
                            switch(level->state)
                            {
                                case Level::State::Combat:
                                    if(itemSchema.canCommandInCombat && this->validateCombat())
                                    {
                                        // Allow the item to apply its operation to the character
                                        Item::useItemOnSelf(character, itemSchema);
                                        character->combat_DecideAction_UsedItem();
                                    }
                                    break;
                                case Level::State::Free:
                                    if(itemSchema.canCommandInFree && this->validateFree())
                                    {
                                        // Allow the item to apply its operation to the character
                                        Item::useItemOnSelf(character, itemSchema);
                                    }
                                    break;
                            }
                            break;
                        } // eo case CommandTypeSelf

                        case Item::CommandType::CommandTypeCharacter:
                        {
                            // Packet will contain the target tile
                            unsigned int tileId;
                            packet >> tileId;

                            // If I have a valid target character to use the item on
                            Tile* t = level->tiles[tileId];
                            if(t && t->body && t->body->schema.type == Body::Type::Character)
                            {
                                Character* other = static_cast<Character*>(t->body);

                                // Apply the item to other character if I am allowed given the current game mode
                                switch(level->state)
                                {
                                    case Level::State::Combat:
                                        if(itemSchema.canCommandInCombat && this->validateCombat())
                                        {
                                            // Allow the item to apply its operation to the character
                                            Item::useItemOnCharacter(character, other, itemSchema);
                                            character->combat_DecideAction_UsedItem();
                                        }
                                        break;
                                    case Level::State::Free:
                                        if(itemSchema.canCommandInFree && this->validateFree())
                                        {
                                            // Allow the item to apply its operation to the character
                                            Item::useItemOnCharacter(character, other, itemSchema);
                                        }
                                        break;
                                }
                                break;
                            }
                        } // eo case CommandTypeCharacter

                        case Item::CommandType::CommandTypeArea:
                        {
                            // Packet will contain the target tile
                            unsigned int tileId;
                            packet >> tileId;

                            // If I have a valid tile
                            Tile* t = level->tiles[tileId];
                            if(t)
                            {
                                // Apply the item to the tile if I am allowed given the current game mode
                                switch(level->state)
                                {
                                    case Level::State::Combat:
                                        if(itemSchema.canCommandInCombat && this->validateCombat())
                                        {
                                            // Allow the item to apply its operation
                                            Item::useItemOnTileArea(character, t, itemSchema);
                                            character->combat_DecideAction_UsedItem();
                                        }
                                        break;
                                    case Level::State::Free:
                                        if(itemSchema.canCommandInFree && this->validateFree())
                                        {
                                            // Allow the item to apply its operation
                                            Item::useItemOnTileArea(character, t, itemSchema);
                                        }
                                        break;
                                }
                                break;
                            }
                            break;
                        } // eo case CommandTypeArea

                    } // eo switch Item::CommandType

                    break;
                } // eo case CommandType::ItemCommand

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
                } // eo case Command::Type::FreeCommand

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
                } // eo case Command::Type::CombatCommand
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
                    spectateNext(-1);
                    break;
                case Command::Type::PlayerMoveRight:
                    spectateNext(1);
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