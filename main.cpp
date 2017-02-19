#include "bitengine/System.hpp"
#include "bitengine/System/tinyxml2.h"
#include "client/RogueZombieGame.hpp"
#include "server/RogueZombieServer.hpp"
#include "ResourcePath.h"

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