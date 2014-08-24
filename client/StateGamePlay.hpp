#pragma once
#ifndef RZ_GAMEPLAYSTATE_H
#define RZ_GAMEPLAYSTATE_H

#include "../bitengine/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"
#include "../server/Command.hpp"
#include "../server/Interaction.hpp"

class LevelClient;
class RogueZombieGame;
class CharacterClient;
class Hud;

class StateGamePlay : public bit::ClientServerState
{
public:

    StateGamePlay(bit::StateStack &stack, RogueZombieGame* game, bool isClient, bool isHost);

    virtual ~StateGamePlay();

    enum Mode
    {
        Init,
        Joining,
        LevelTransition,
        Free,
        Combat,
        Interact,
        Dialog,
        Loot,
        Inventory,
        Journal,
        Trade,
        _count
    };

    enum EndGameReason
    {
        Quit,
        Victory,
        Defeat
    };

    struct Target
    {
        Target() : tileId(0), bodyId(0) {}

        unsigned int tileId;
        unsigned int bodyId;

        void reset();
        bool active();
    };

    RogueZombieGame* rogueZombieGame;
    Mode mode;
    EndGameReason endGameReason;
    std::vector<std::function<void()>> modeEnter;
    std::vector<std::function<void(sf::Time&)>> modeUpdate;
    std::vector<std::function<void()>> modeExit;
    LevelClient* levelClient;
    std::vector<Command> commandQueue;
    sf::Vector2f mousePositionInLevel;
    bit::FrameTimer fps;
    Target target;

    // Test Gui
    Hud* hud;

    virtual void load();

    void now();

    void changeMode(Mode);

    void modeOnEnterJoining();
    void modeOnExitJoining();
    void modeOnUpdateJoining(sf::Time &gameTime);

    void modeOnEnterFree();
    void modeOnExitFree();
    void modeOnUpdateFree(sf::Time &gameTime);
    
    void modeOnEnterLoot();
    void modeOnExitLoot();
    void modeOnUpdateLoot(sf::Time &gameTime);
    
    void modeOnEnterInteract();
    void modeOnExitInteract();
    void modeOnUpdateInteract(sf::Time &gameTime);
    
    void modeOnEnterDialog();
    void modeOnExitDialog();
    void modeOnUpdateDialog(sf::Time &gameTime);

    void modeOnEnterInventory();
    void modeOnExitInventory();
    void modeOnUpdateInventory(sf::Time &gameTime);

    void modeOnEnterJournal();
    void modeOnExitJournal();
    void modeOnUpdateJournal(sf::Time &gameTime);

    void modeOnUpdateCommonListener(sf::Time &gameTime);

    void onEnterCombat();
    void onLeaveCombat();

    void issueCommand(Command cmd);

    virtual bool handleInput(sf::Time &gameTime);

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

    void displayMessage(std::string &message);

    void displayPlayerMessage(CharacterClient* character, std::string &message);

    void requestInteractionsForTile(unsigned int tileId);

    void handleInteractionResponse(unsigned int tileId, Interaction::Type interaction, bit::ServerPacket &packet);

    void endGame(EndGameReason);

protected:

    virtual bit::Server* newServer();

    virtual std::string getServerIpAddress();

    virtual unsigned short getServerPort();

    // Packet Handling

    virtual void handlePacket_Broadcast(bit::ServerPacket &packet);

    virtual void handlePacket_InitializeSelf(bit::ServerPacket &packet);

    virtual void handlePacket_InitializeWorld(bit::ServerPacket &packet);

    virtual void handlePacket_PeerClientConnected(bit::ServerPacket &packet);

    virtual void handlePacket_ClientDisonnected(bit::ServerPacket &packet);

    virtual void handlePacket_ServerUpdate(bit::ServerPacket &packet);

    virtual void handlePacket_ServerEvent(bit::ServerPacket &packet);

    virtual void handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet);

    virtual void handlePacket_Shutdown(bit::ServerPacket &packet);

    virtual void handle_DisconnectTimeout();

    virtual void handlePacket_Kick(bit::ServerPacket &packet);

    virtual void handle_ServerTimeout();

    // Packet Sending

    virtual void preparePacket_ClientInformation(bit::ClientPacket &packet);

    virtual void preparePacket_ClientUpdate(bit::ClientPacket &packet);

    virtual void preparePacket_ClientDisconnect(bit::ClientPacket &packet);
};

#endif