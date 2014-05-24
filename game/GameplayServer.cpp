#include "GameplayServer.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/System.hpp"
#include "ClientRequest.hpp"
#include "Command.hpp"
#include "Player.hpp"
#include "Interaction.hpp"
#include "Character.hpp"
#include "characters/Hunter.hpp"
#include "Tile.hpp"
#include "levels/Interior.hpp"
#include "mission/Mission.hpp"
#include "items/Item.hpp"
#include "dialog/Dialog.hpp"

GameplayServer::GameplayServer()
    : bit::Server(), bodyIdCounter(0), missionIdCounter(0), itemIdCounter(0), dialogIdCounter(0)
{
}

GameplayServer::~GameplayServer()
{
    for(auto iterator = players.begin(); iterator != players.end(); iterator++)
    {
        delete iterator->second;
    }
}

void GameplayServer::load()
{
    levels.resize(2);
    for(unsigned int i=0; i < levels.size(); i++)
    {
        switch(i)
        {
            case 0:
            {
                levels[i].load(this, i, Interior::level_01, Interior::level_01_rows, Interior::level_01_cols);
                break;
            }
            case 1:
            {
                levels[i].load(this, i, Interior::level_02, Interior::level_02_rows, Interior::level_02_cols);
                break;
            }
        }
    }
}

void GameplayServer::update(sf::Time &gameTime)
{
    // Serve requests
    for(unsigned int i=0; i < pendingMoves.size(); i++)
    {
        pendingMoves[i].player->requestFullSnapshot = true;
        levels[pendingMoves[i].fromLevelId].removePlayer(pendingMoves[i].player);
        levels[pendingMoves[i].toLevelId].addPlayer(pendingMoves[i].player);
    }
    pendingMoves.clear();

    for(unsigned int i=0; i < levels.size(); i++)
    {
        levels[i].update(gameTime);
    }
}

unsigned int GameplayServer::getNextBodyId()
{
    return ++bodyIdCounter;
}

unsigned int GameplayServer::getNextMissionId()
{
    return ++missionIdCounter;
}

unsigned int GameplayServer::getNextItemId()
{
    return ++itemIdCounter;
}

unsigned int GameplayServer::getNextDialogId()
{
    return ++dialogIdCounter;
}

void GameplayServer::movePlayerToLevel(Player* player, unsigned int fromLevelId, unsigned int toLevelId)
{
    PendingMovePlayer m;
    m.player = player;
    m.fromLevelId = fromLevelId;
    m.toLevelId = toLevelId;
    pendingMoves.push_back(m);
}

/**
 * Handle Incoming Client Packets
 **/

void GameplayServer::handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	if(players.find(client.id) == players.end())
    {
        // Player creation
        Player* p = new Player();
        p->load(&client);
        players[client.id] = p;

        if(!levels[0].createPlayer(p))
        {
            bit::Output::Debug("Server cannot create player, no room in the inn");
            players.erase(client.id);
            kickClient(client, KickReason::NoSpawn);
        }

        // Mission number 1
        Mission* root = new Mission();
        root->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::TestMissionRoot);

        Mission* healthMission = new Mission();
        healthMission->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::GetDoubleHealth);
        healthMission->assignRequirement([] (Character* c) -> bool {
            return (c->schema.health >= 200);
        });
        root->assignChildMission(healthMission);

        Mission* levelMission = new Mission();
        levelMission->load(getNextMissionId(), LogicalType::Selector, Mission::GenerationType::Scripted, JournalEntry::Entry::FindLevelTwo);
        levelMission->assignRequirement([] (Character* c) -> bool {
            return (c->level == &c->level->server->levels[1]);
        });
        root->assignChildMission(levelMission);

        p->character->assignMission(root);

        // Items
        Item* magnum = Item::create(Item::Type::Magnum357, getNextItemId());
        p->character->addItemToInventory(magnum);
    }
}

void GameplayServer::handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    //bit::Output::Debug("Server handle client update");
    Player* player = players[client.id];

	// Get command count
	sf::Uint32 commandCount;
	packet >> commandCount;

	for(unsigned int i=0; i < commandCount; i++)
	{
		// Get command
		sf::Uint8 commandType;
		packet >> commandType;

		// Determine how to handle
		switch(commandType)
		{
            default:
                player->level->handlePlayerCommand(packet, client, static_cast<Command::Type>(commandType));
				break;
            case Command::Type::PlayerDebug:
                if(player->level->state == Level::State::Free)
                {
                    player->character->isHostileCombatDetected = true;
                    player->level->enterCombat();
                }
                else
                {
                    player->character->isHostileCombatDetected = false;
                }

                break;
		}
	}
}

void GameplayServer::handlePacket_ClientDisconnect(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client disconnect");

    // Remove them from their last level
    auto itr = players.find(client.id);
    if(itr != players.end())
    {
        Player* p = itr->second;
        players.erase(client.id);
        p->level->deletePlayer(p);
    }
}

void GameplayServer::handle_ClientTimeout(bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client timeout");

    // Remove them from their last level
    auto itr = players.find(client.id);
    if(itr != players.end())
    {
        Player* p = itr->second;
        players.erase(client.id);
        p->level->deletePlayer(p);
    }
}

void GameplayServer::handlePacket_ClientRequest(bit::ClientPacket &packet, bit::RemoteClient &client, bit::ServerPacket &responsePacket)
{
    bit::Output::Debug("Server handle client request");
    Player* player = players[client.id];

    ClientRequest request;
    bit::NetworkHelper::unpackEnum<sf::Uint32, ClientRequest>(packet, request);
    switch(request)
    {
        case ClientRequest::Interaction:
        {
            bit::Output::Debug("Server detect request interaction");

            Interaction::Type interaction;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Interaction::Type>(packet, interaction);
            unsigned int tileID;
            packet >> tileID;
            Tile* t = player->level->getTileById(tileID);
            if(t && t->body)
            {
                t->body->handleInteraction(interaction, player->character, responsePacket);
            }

            break;
        }
        case ClientRequest::GetInteractionOptions:
        {
            bit::Output::Debug("Server detect request interaction options");

            unsigned int tileId;
            packet >> tileId;
            Tile* t = player->level->getTileById(tileId);

            if(t && t->body && t->body->Body::schema.id != player->character->Body::schema.id)
            {
                std::vector<Interaction::Type> interactions;
                t->body->getAvailableInteractions(interactions);

                if(interactions.size() > 0)
                {
                    responsePacket << true;
                    responsePacket << sf::Uint32(interactions.size());
                    for(unsigned int i=0; i < interactions.size(); i++)
                    {
                        responsePacket << sf::Uint32(interactions[i]);
                    }
                }
                else
                {
                    responsePacket << false;
                }
            }
            else
            {
                responsePacket << false;
            }

            break;
        }
        case ClientRequest::GetCharacterStatistics:
        {
            bit::Output::Debug("Server detect request character statistics");

            unsigned int tileId;
            packet >> tileId;
            Tile* t = player->level->getTileById(tileId);

            if(t && t->body && t->body->Body::schema.id != player->character->Body::schema.id && t->body->schema.type == Body::Type::Character)
            {
                Character* c = static_cast<Character*>(t->body);

                if(!c->schema.isDead())
                {
                    responsePacket << true;
                    responsePacket << sf::Uint32(c->schema.type);
                    responsePacket << sf::Uint32(c->schema.health);
                    responsePacket << sf::Uint32(c->schema.maxHealth);
                    responsePacket << sf::Uint32(c->schema.speed);
                }
                else
                {
                    responsePacket << false;
                }
            }
            else
            {
                responsePacket << false;
            }

            break;
        }
        case ClientRequest::DialogResponse:
        {
            bit::Output::Debug("Server detect request dialog response");

            Dialog response;
            packet >> response;

            if(player->character->conversationSpeaker)
            {
                player->character->conversationSpeaker->handleDialogResponse(player->character, response.id);
                player->character->conversationSpeaker->prepareDialogNode(player->character, responsePacket);
            }

            break;
        }
        case ClientRequest::TransferItem:
        {
            bit::Output::Debug("Server detect request transfer item");

            unsigned int itemId;
            packet >> itemId;

            if(player->character->inventoryAccessee)
            {
                // Remove the item from the current accessee and then add it to the player
                Item* item = player->character->inventoryAccessee->removeItemFromInventory(itemId);
                if(item)
                {
                    player->character->addItemToInventory(item);
                    responsePacket << true;
                }
                else
                {
                    responsePacket << false;
                }
            }
            else
            {
                responsePacket << false;
            }

            break;
        }
        case ClientRequest::CloseInventory:
        {
            bit::Output::Debug("Server detect request close inventory");

            unsigned int itemId;
            packet >> itemId;

            if(player->character->inventoryAccessee)
            {
                player->character->inventoryAccessee->inventoryAccessor = NULL;
                player->character->inventoryAccessee = NULL;
            }

            break;
        }
        case ClientRequest::EquipItem:
        {
            bit::Output::Debug("Server detect request equip item request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);
            unsigned int itemId;
            packet >> itemId;

            responsePacket << player->character->equipFromInventory(slot, itemId);

            break;
        }
        case ClientRequest::UnequipItem:
        {
            bit::Output::Debug("Server detect request unequip item request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);

            player->character->unequip(slot);

            break;
        }
    }
}

/**
 * Prepare Outgoing Server Packets
 **/

void GameplayServer::preparePacket_InitializeSelf(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare initialize self");
}

void GameplayServer::preparePacket_InitializeWorld(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare initialize level");

    Player* player = players[client.id];
    packet << sf::Uint32(player->level->id);
    player->level->prepareSnapshot(packet, client, true);
}

void GameplayServer::preparePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
}

void GameplayServer::preparePacket_PeerClientDisconnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
}

void GameplayServer::preparePacket_ServerUpdate(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    //bit::Output::Debug("Server prepare server update");

    Player* player = players[client.id];

    packet << sf::Uint32(player->level->id);
    packet << player->requestFullSnapshot;
    if(player->requestFullSnapshot) bit::Output::Debug("sending full");
    player->level->prepareSnapshot(packet, client, player->requestFullSnapshot);

    if(player->requestFullSnapshot)
        player->requestFullSnapshot = false;
}

void GameplayServer::preparePacket_DisconnectAcknowledge(bit::ServerPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server prepare server disconnect acknowledge");
}

void GameplayServer::preparePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare server shutdown");
}