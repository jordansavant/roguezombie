#pragma once
#ifndef BIT_INPUTMANAGER_H
#define BIT_INPUTMANAGER_H

#include "SFML/Graphics.hpp"
#include "../Game/GameComponent.hpp"
#include "XinputGamepad.hpp"
#include "InputBinding.hpp"
#include "InputBindingVector.hpp"

namespace bit
{
    class Game;

	class InputManager : public GameComponent
	{
	public:

		InputManager(Game* game);

        virtual ~InputManager();

        virtual void preinspectEvents();

        virtual void inspectMouseWheelEvent(sf::Event e);

		virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        // Mouse
		bool isButtonDown(sf::Mouse::Button mouseButton);

		bool isButtonPressed(sf::Mouse::Button mouseButton);

        bool isButtonReleased(sf::Mouse::Button mouseButton);

        // Keyboard
        bool isButtonDown(sf::Keyboard::Key keyboardKey);

        bool isButtonPressed(sf::Keyboard::Key keyboardKey);

        bool isButtonReleased(sf::Keyboard::Key keyboardKey);

        // Gamepad
        bool isButtonDown(XinputGamepad::Button gamepadButton);

        bool isButtonPressed(XinputGamepad::Button gamepadButton);

        bool isButtonReleased(XinputGamepad::Button gamepadButton);

        int getMouseWheelTicks();

        sf::Vector2f getVector(XinputGamepad::Vector vector);

        bool isButtonDown(int gamepadIndex, XinputGamepad::Button gamepadButton);

        bool isButtonPressed(int gamepadIndex, XinputGamepad::Button gamepadButton);

        bool isButtonReleased(int gamepadIndex, XinputGamepad::Button gamepadButton);

        sf::Vector2f getVector(int gamepadIndex, XinputGamepad::Vector vector);

        // Keyboard Vectors
        sf::Vector2f getInputBindingVectorVector(unsigned int index);

        int assignInputBindingVector(InputBindingVector* keyboardVector);

        int reassignInputBindingVector(InputBindingVector* keyboardVector);

        // Button Bindings
        bool isButtonDown(std::vector<InputBinding*> &bindings);

        bool isButtonDown(InputBinding &binding);

        bool isButtonPressed(std::vector<InputBinding*> &binding);

        bool isButtonPressed(InputBinding &binding);

        bool isButtonReleased(std::vector<InputBinding*> &binding);

        bool isButtonReleased(InputBinding &binding);

        sf::Vector2f getVector(std::vector<InputBinding*> &binding);

        sf::Vector2f getVector(InputBinding &binding);

	private:
		bool mouseButtonLastDown[5];
		bool mouseButtonCurrentDown[5];
		bool keyboardKeyLastDown[101];
		bool keyboardKeyCurrentDown[101];
        int mouseWheelCurrentDelta;
        XinputGamepad gamepads[8];
        static const int gamepadCount = 8;
        int gamepadConnectionCounter;
        std::vector<InputBindingVector*> inputBindingVectors;
	};
}

#endif