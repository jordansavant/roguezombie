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
#include "ServerEvent.hpp"

GameplayServer::GameplayServer()
    : bit::Server(), startingLevel(NULL), isGameOver(false), bodyIdCounter(0), missionIdCounter(0), itemIdCounter(0), dialogIdCounter(0)
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
    // Load the level pack definitions
    levelLoader.load("RZ2.xml");

    // Build all levels for this level pack
    levels.resize(levelLoader.levelDefs.size());
    for(unsigned int i=0; i < levelLoader.levelDefs.size(); i++)
    {
        levels[i].load(this, levelLoader.levelDefs[i]);

        // Set up starting level for players
        if(levelLoader.defaultLevelId == levels[i].id || startingLevel == NULL)
        {
            startingLevel = &levels[i];
        }
    }
}

void GameplayServer::update(sf::Time &gameTime)
{
    // Honor player join requests
    for(unsigned int i=0; i < pendingJoins.size(); i++)
    {
        Player* p = pendingJoins[i].player;
        if(startingLevel->createPlayer(p))
        {
            p->setupPlayerCharacter();
            sendWorldInitialization(*p->client);
            pendingJoins[i].complete = true;
        }
        else if(pendingJoins[i].kickTimer.update(gameTime))
        {
            bit::Output::Debug("Server cannot create player, no room in the inn");
            players.erase(p->clientId);
            kickClient(*p->client, KickReason::NoSpawn);
        }
    }
    // Remove completed requests
    pendingJoins.erase(std::remove_if(pendingJoins.begin(), pendingJoins.end(), [](PendingPlayerJoin p) { return p.complete; }), pendingJoins.end());

    // Honor level transition requests
    for(unsigned int i=0; i < pendingMoves.size(); i++)
    {
        PendingMovePlayer* pm = &pendingMoves[i];

        unsigned int indexFrom = pm->fromLevelId - 1;
        unsigned int indexTo = pm->toLevelId - 1;

        // If I can add the player to the new level
        if(levels[indexTo].canAddPlayer(pm->player, pm->toEntranceId))
        {
            // Remove them from their current level
            levels[indexFrom].removePlayer(pm->player);

            // Add them to their new level
            levels[indexTo].addPlayer(pm->player, pm->toEntranceId);
            
            // Tell player to get full snapshot
            pm->player->requestFullSnapshot = true;
            // If the player character has spectators tell them as well
            for(unsigned int i=0; i < pm->player->character->spectators.size(); i++)
            {
                Player* spectator = pm->player->character->spectators[i];
                spectator->requestFullSnapshot = true;
            }

            // Notify client of transition
            sendEventToClient(*pm->player->client, [](bit::ServerPacket &packet){
                packet << sf::Uint32(ServerEvent::ArrivedLevel);
            });

            pm->complete = true;
        }
    }
    // Remove completed requests
    pendingMoves.erase(std::remove_if(pendingMoves.begin(), pendingMoves.end(), [](PendingMovePlayer p) { return p.complete; }), pendingMoves.end());

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

void GameplayServer::movePlayerToLevel(Player* player, unsigned int fromLevelId, unsigned int toLevelId, unsigned int toEntranceId)
{
    // Queue the move
    PendingMovePlayer m;
    m.player = player;
    m.fromLevelId = fromLevelId;
    m.toLevelId = toLevelId;
    m.toEntranceId = toEntranceId;
    pendingMoves.push_back(m);

    // Notify client of transition
    sendEventToClient(*player->client, [](bit::ServerPacket &packet){
        packet << sf::Uint32(ServerEvent::LeavingLevel);
    });
}

void GameplayServer::deletePlayerFromLevelAndServer(Player* p)
{
    players.erase(p->clientId);
    if(p->level)
        p->level->deletePlayer(p); // level only exits if player actually joined
    delete p;
    
    // check if game over if a player leaves for any reason
    checkForGameOver();
}

Level* GameplayServer::getNextLoadedLevel(Level* fromLevel)
{
    int nextIndex = fromLevel->id + 1 - 1; // next level id -1 for index
    if(nextIndex < levels.size())
        return &levels[nextIndex];
    else
        return &levels.front();
}

Level* GameplayServer::getPreviousLoadedLevel(Level* fromLevel)
{
    int nextIndex = fromLevel->id - 1 - 1; // next level id -1 for index
    if(nextIndex >= 0)
        return &levels[nextIndex];
    else
        return &levels.back();
}

// If all players have no living characters its game over
void GameplayServer::checkForGameOver()
{
    if(isGameOver)
        return;

    // Iterate the levels
    bool alive = false;
    for(auto iterator = players.begin(); iterator != players.end(); iterator++)
    {
        Player* player = iterator->second;
        if(player->character && !player->character->schema.isDead())
        {
            alive = true;
        }
    }

    if(!alive)
    {
        // If everyone pathetically died like losers...
        sendEventToAllClients([](bit::ServerPacket& packet){
            packet << sf::Uint32(ServerEvent::GameDefeat);
        });
        isGameOver = true;
    }
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

        // Setup join request
        PendingPlayerJoin pending;
        pending.player = p;
        pendingJoins.push_back(pending);
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
                player->character->harm(20);
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
        deletePlayerFromLevelAndServer(p);
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
        deletePlayerFromLevelAndServer(p);
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
        case ClientRequest::ProcessInteractionForBodyOnTile:
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

        case ClientRequest::GetAvailableInteractionsForBodyOnTile:
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

        case ClientRequest::RespondToDialog:
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

        case ClientRequest::TransferItemFromCounterpartyToParty:
        {
            bit::Output::Debug("Server detect request transfer item");

            unsigned int itemId;
            packet >> itemId;

            if(player->character->inventoryHost)
            {
                // Remove the item from the current accessee and then add it to the player
                Item* item = player->character->inventoryHost->removeItemFromInventory(itemId);
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

        case ClientRequest::CloseItemTransaction:
        {
            bit::Output::Debug("Server detect request close inventory");

            player->character->closeInventoryOfHost();

            break;
        }

        case ClientRequest::EquipItemFromInventoryToSlot:
        {
            bit::Output::Debug("Server detect request equip item from inventory request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);
            unsigned int itemId;
            packet >> itemId;

            responsePacket << player->character->equipFromInventory(slot, itemId);

            break;
        }

        case ClientRequest::UnequipItemFromSlotToInventory:
        {
            bit::Output::Debug("Server detect request unequip item request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);

            player->character->unequip(slot);

            break;
        }
        
        case ClientRequest::EquipItemFromLootToSlot:
        {
            bit::Output::Debug("Server detect request equip item from loot request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);
            unsigned int itemId;
            packet >> itemId;

            responsePacket << player->character->equipFromLoot(slot, itemId);

            break;
        }

        case ClientRequest::MoveInventoryItemToPosition:
        {
            bit::Output::Debug("Server detect request move item request");
            
            unsigned int itemId;
            packet >> itemId;
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveItemToPosition(itemId, position);

            break;
        }

        case ClientRequest::MoveEquippedItemToInventoryPosition:
        {
            bit::Output::Debug("Server detect request move item from equipment request");

            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveEquipmentToPosition(slot, position);

            break;
        }

        case ClientRequest::SwapEquipmentBySlot:
        {
            bit::Output::Debug("Server detect request equipment swap request");
            
            Character::EquipmentSlot slotA;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slotA);

            Character::EquipmentSlot slotB;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slotB);

            responsePacket << player->character->swapEquipment(slotA, slotB);

            break;
        }

        case ClientRequest::MoveLootItemToInventoryPosition:
        {
            bit::Output::Debug("Server detect request move loot to inventory request");
            
            unsigned int itemId;
            packet >> itemId;
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveLootItemToInventoryPosition(itemId, position);

            break;
        }

        case ClientRequest::MoveInventoryItemToLootPosition:
        {
            bit::Output::Debug("Server detect request move inventory to loot request");
            
            unsigned int itemId;
            packet >> itemId;
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveInventoryItemToLootPosition(itemId, position);

            break;
        }

        case ClientRequest::MoveLootItemToLootPosition:
        {
            bit::Output::Debug("Server detect request move loot to loot request");
            
            unsigned int itemId;
            packet >> itemId;
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveLootItemToLootPosition(itemId, position);

            break;
        }

        case ClientRequest::MoveEquippedItemToLootPosition:
        {
            bit::Output::Debug("Server detect request move equipment to loot request");
            
            Character::EquipmentSlot slot;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::EquipmentSlot>(packet, slot);
            unsigned int position;
            packet >> position;

            responsePacket << player->character->moveEquipmentToLootPosition(slot, position);

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