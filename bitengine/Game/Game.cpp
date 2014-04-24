#include "Game.hpp"
#include "GameComponent.hpp"
#include "StateStack.hpp"

bit::Game::Game()
{
    running = true;
	actualTime = clock.getElapsedTime().asSeconds();
	timer = 0.0f;
	FPS = 60.0f;
	dt = 1.0f / FPS;

    stateStack = new StateStack(this);
    gameComponents.push_back(stateStack);
}

bit::Game::~Game()
{
    delete stateStack;
}

void bit::Game::run()
{
    registerStates();

	// Game loop
	running = true;
	while (running)
	{
		// UPDATE at fixed intervals
		float newTime = clock.getElapsedTime().asSeconds();
		float frameTime = newTime - actualTime;
		if(frameTime > 0.25f)
		{
			frameTime = 0.25f; // Avoid spiral of death
		}
		timer += frameTime;

		// Run update in dt sized chunks
		while(timer >= dt)
		{
            sf::Time gtu = sf::seconds(dt);
 			update(gtu);
			timer -= dt;
		}

		actualTime = newTime;
	}
}

void bit::Game::update(sf::Time &gameTime)
{
    // Update various managers and components
	for(unsigned int i=0; i<gameComponents.size(); i++)
	{
		gameComponents[i]->update(gameTime);
	}

    // Quit if no states
    if(stateStack->empty())
	{
		quit();
	}
}

void bit::Game::registerStates()
{
}

void bit::Game::quit()
{
    running = false;
}