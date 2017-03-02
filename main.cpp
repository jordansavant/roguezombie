#include "bitengine/System.hpp"
#include "bitengine/System/tinyxml2.h"
#include "client/RogueZombieGame.hpp"
#include "server/RogueZombieServer.hpp"
#include "xogeni/XoGeniGame.hpp"
#include "ResourcePath.h"

#if defined(__OS_WIN__)
#if defined(_DEBUG)
#include "vld.h"
#endif
#endif

int main(int argc, char* argv[])
{
    srand(time(NULL));

    XoGeniGame generator;
    generator.run();

    return 0;

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