#include <functional>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "ResourcePath.h"
#include "bitengine/Math/Random.hpp"
#include "bitengine/System/Output.hpp"
#include "superpants/TestGame.hpp"

int main()
{
    srand (time(NULL));
	TestGame game;
	game.run();

	return 0;
}

int notmain()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

    std::stringstream ss;

    bit::Random random(123);
    ss << random.min() << " - " << random.max() << std::endl;
    bit::Output::Debug(ss.str());
    for(int i=0; i < 5; i++)
    {
        ss << random.next() << std::endl << random.nextFloat() << std::endl;
        bit::Output::Debug(ss.str());
    }

    sf::Texture zombieimage;
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    sf::Sprite zombiesprite;
    zombiesprite.setTexture(zombieimage);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
        window.draw(zombiesprite);
		window.display();
	}

	return 0;
}
