#include <functional>
#include <SFML/Graphics.hpp>
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
		window.display();
	}

	return 0;
}
