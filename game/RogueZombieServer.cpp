#include "RogueZombieServer.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "StateGameStart.hpp"
#include "StateServerPlay.hpp"
#include <iostream>
#include <sstream>

RogueZombieServer::RogueZombieServer()
	: bit::Game()
{
    Game::stateStack->pushState(stateServerPlay);
}

unsigned int RogueZombieServer::stateServerPlay = 1;

void RogueZombieServer::update(sf::Time &gameTime)
{
	bit::Game::update(gameTime);
}

void RogueZombieServer::registerStates()
{
    stateStack->registerState<StateServerPlay, RogueZombieServer>(this, stateServerPlay);
}
