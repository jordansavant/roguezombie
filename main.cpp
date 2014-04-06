#include "bitengine/System.hpp"
#include "game/RogueZombieGame.hpp"

int main(int argc, char* argv[])
{
    bit::Output::Debug(argc);
    srand(time(NULL));
	RogueZombieGame game;
	game.run();

	return 0;
}