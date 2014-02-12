#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "cpd.h"
#include "ResourcePath.h"
#include "bitengine/Foo.h"
#include "bitengine/Bar.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

    bit::Foo foo;
    foo.bar();

    bit::Bar bar;
    bar.foo();
    int r = bar.lamb([] (int tar) -> bool {
        return true;
    });
    r++;

    std::cout << cpd::Dir::ExecutablePath() << std::endl;
    std::cout << cpd::Dir::ExecutableDir() << std::endl;

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
