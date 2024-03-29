#include "bitengine/System.hpp"
#include "bitengine/Math.hpp"
#include "bitengine/System/tinyxml2.h"
#include "client/RogueZombieGame.hpp"
#include "server/RogueZombieServer.hpp"
#include "xogeni/XoGeniGame.hpp"
#include "ResourcePath.h"

#if defined(__OS_WIN__)
#if defined(_DEBUG)
//#include "vld.h" // Visual Leak Detector (Memory Leaks)
#endif
#endif


int main(int argc, char* argv[])
{
    bit::Output::stream << "Test" << ": " << bit::Math::toHash(std::string("Test")); bit::Output::Flush();
    bit::Output::stream << "Test" << ": " << bit::String::toHash(std::string("Test")); bit::Output::Flush();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
    {
        XoGeni::XoGeniGame generator;
        generator.run();

        return 0;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        RogueZombieServer server;
        server.run();

        return 0;
    }

    RogueZombieGame game;
    game.run();

    return 0;
}