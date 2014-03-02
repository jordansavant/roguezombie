#include "game/RogueZombieGame.hpp"

int main()
{
    srand(time(NULL));
	RogueZombieGame game;
	game.run();

	return 0;
}