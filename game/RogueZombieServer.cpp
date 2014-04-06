#include "RogueZombieServer.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "StartMenuState.hpp"
#include "ServerOnlyState.hpp"
#include <iostream>
#include <sstream>

RogueZombieServer::RogueZombieServer()
	: bit::Game()
{
    Game::stateStack->pushState(serverState);
}

unsigned int RogueZombieServer::serverState = 1;

void RogueZombieServer::update(sf::Time &gameTime)
{
	bit::Game::update(gameTime);
}

void RogueZombieServer::registerStates()
{
    stateStack->registerState<ServerOnlyState, RogueZombieServer>(this, serverState);
}
