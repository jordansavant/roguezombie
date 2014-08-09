#include "bitengine/System.hpp"
#include "bitengine/System/tinyxml2.h"
#include "client/RogueZombieGame.hpp"
#include "server/RogueZombieServer.hpp"
#include "ResourcePath.h"

int main(int argc, char* argv[])
{
    // test tinyxml
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile((resourcePath() + "RZ2.xml").c_str());
    tinyxml2::XMLElement* lp = doc.FirstChildElement( "levelpack" );
    tinyxml2::XMLElement* nameElement = lp->FirstChildElement( "name" );
	const char* title = nameElement->GetText();
    bit::Output::Debug(std::string(title));

    bit::Output::Debug(argc);
    srand(time(NULL));

    if(true || argc == 1)
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