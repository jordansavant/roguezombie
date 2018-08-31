#pragma once
#ifndef RZ_GAMEPLAYSTATE_H
#define RZ_GAMEPLAYSTATE_H

#include "../bitengine/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"
#include "../server/Command.hpp"
#include "../server/Interaction.hpp"
#include "../server/items/Item.hpp"

class LevelClient;
class RogueZombieGame;
class CharacterClient;
class Hud;
class JoiningMenu;
class TileClient;

class StateGamePlay : public bit::ClientServerState
{
public:

    StateGamePlay(bit::StateStack &stack, RogueZombieGame* game, bool isClient, bool isHost, bool isLocalOnly);

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
        Options,
		Dev,
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
    std::vector<std::function<void(sf::Time&)>> modeCaptureInput;
    std::vector<std::function<void(sf::Time&)>> modeUpdate;
    std::vector<std::function<void()>> modeExit;
    LevelClient* levelClient;
    std::vector<Command> commandQueue;
    sf::Vector2f mousePositionInLevel;
    bit::FrameTimer fps;
    bool isHudInteractionOccurring;
    bool input_tileSelect;
    bool inputActive_tileSelect;
    bool isShiftModifierDown;
    Target target;
    std::vector<sf::Vector2f> interactionDirectionChecks;
    unsigned int lastInteractionDirectionCheckIndex;
    
    unsigned int explosionSoundId;
    unsigned int pistolGunshotSoundId;
    unsigned int rifleGunshotSoundId;
    unsigned int doorCloseSoundId;
    unsigned int openBodySoundId;
    unsigned int closeBodySoundId;
    unsigned int openChestSoundId;
    unsigned int closeChestSoundId;
    unsigned int humanCharacterDeathSoundId;
    unsigned int thudSoundId;
    unsigned int healSoundId;
    unsigned int spikeReleaseSoundId;
    unsigned int spikeReturnSoundId;
    unsigned int whiffSoundId;

    // Gui
    Hud* hud;
    JoiningMenu* joiningMenu;

    virtual void load();

    void setGameZoom(bool setZoom);

    bool isGameZoomed();

    float getZoomRatio();

    void switchLevels(unsigned int newId);

    void onHoverTile(TileClient* tileClient);

    void onUnhoverTile(TileClient* tileClient);

    void now();

    void exitInventoryModes();

    void changeMode(Mode);

    void modeOnEnterJoining();
    void modeOnExitJoining();
    void modeOnCaptureInputJoining(sf::Time &gameTime);
    void modeOnUpdateJoining(sf::Time &gameTime);

    void modeOnEnterFree();
    void modeOnExitFree();
    void modeOnCaptureInputFree(sf::Time &gameTime);
    void modeOnUpdateFree(sf::Time &gameTime);
    
    void modeOnEnterLoot();
    void modeOnExitLoot();
    void modeOnCaptureInputLoot(sf::Time &gameTime);
    void modeOnUpdateLoot(sf::Time &gameTime);
    
    void modeOnEnterInteract();
    void modeOnExitInteract();
    void modeOnCaptureInputInteract(sf::Time &gameTime);
    void modeOnUpdateInteract(sf::Time &gameTime);
    
    void modeOnEnterDialog();
    void modeOnExitDialog();
    void modeOnCaptureInputDialog(sf::Time &gameTime);
    void modeOnUpdateDialog(sf::Time &gameTime);

    void modeOnEnterInventory();
    void modeOnExitInventory();
    void modeOnCaptureInputInventory(sf::Time &gameTime);
    void modeOnUpdateInventory(sf::Time &gameTime);

    void modeOnEnterJournal();
    void modeOnExitJournal();
    void modeOnCaptureInputJournal(sf::Time &gameTime);
    void modeOnUpdateJournal(sf::Time &gameTime);

    void modeOnEnterOptions();
    void modeOnExitOptions();
    void modeOnCaptureInputOptions(sf::Time &gameTime);
    void modeOnUpdateOptions(sf::Time &gameTime);

	void modeOnEnterDev();
	void modeOnExitDev();
	void modeOnCaptureInputDev(sf::Time &gameTime);
	void modeOnUpdateDev(sf::Time &gameTime);

    void modeOnCaptureInputCommonListener(sf::Time &gameTime);
    void modeOnUpdateCommonListener(sf::Time &gameTime);

    void onEnterCombat();
    void onLeaveCombat();

    void requestItemCommand(Item::Schema &itemSchema, std::function<void()> onComplete = NULL);

    void issueCommand(Command cmd);

    virtual bool handleInput(sf::Time &gameTime);

    void captureModelessInput(sf::Time &gameTime);

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

    void displayMessage(std::string const&message);

    void displayPlayerMessage(CharacterClient* character, std::string const&message);

    void requestInteractionsForTile(unsigned int tileId);

    TileClient* findNextInteractableTile();

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