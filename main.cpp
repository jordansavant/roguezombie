#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "ResourcePath.h"
#include "bitengine/Structures/Exception.hpp"
#include "bitengine/Math/Random.hpp"

int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

    try
    {
        throw bit::Exception("Sample exception here.");
    }
    catch(bit::Exception &e)
    {
        std::cout << e.what() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }

    bit::Random random;
    std::cout << random.min() << " - " << random.max() << std::endl;
    for(int i=0; i < 5; i++)
    {
        std::cout << random.next() << std::endl << random.nextFloat() << std::endl;
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
