#include "bitengine/System.hpp"
#include "game/RogueZombieGame.hpp"
#include "game/RogueZombieServer.hpp"

int main(int argc, char* argv[])
{
    bit::Output::Debug(argc);
    srand(time(NULL));

    if(argc == 1)
    {
	    RogueZombieGame game;
	    game.run();
    }
    else
    {
	    RogueZombieServer server;
        server.run();
    }

	return 0;
}