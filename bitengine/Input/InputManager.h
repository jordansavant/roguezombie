#pragma once
#ifndef BIT_INPUTMANAGER_H
#define BIT_INPUTMANAGER_H

#include "SFML/Graphics.hpp"
#include "../Game/GameComponent.h"
#include "XinputGamepad.h"
#include "InputBinding.h"
#include "InputBindingVector.h"

namespace bit
{
	class InputManager : public GameComponent
	{
	public:

		InputManager();

        virtual ~InputManager();

        virtual void preinspectEvents();

        virtual void inspectMouseWheelEvent(sf::Event e);

		virtual void update(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused);

        // Mouse
		bool InputManager::isButtonDown(sf::Mouse::Button mouseButton);

		bool InputManager::isButtonPressed(sf::Mouse::Button mouseButton);

        bool InputManager::isButtonReleased(sf::Mouse::Button mouseButton);

        // Keyboard
        bool InputManager::isButtonDown(sf::Keyboard::Key keyboardKey);

        bool InputManager::isButtonPressed(sf::Keyboard::Key keyboardKey);

        bool InputManager::isButtonReleased(sf::Keyboard::Key keyboardKey);

        // Gamepad
        bool InputManager::isButtonDown(XinputGamepad::Button gamepadButton);

        bool InputManager::isButtonPressed(XinputGamepad::Button gamepadButton);

        bool InputManager::isButtonReleased(XinputGamepad::Button gamepadButton);

        int InputManager::getMouseWheelTicks();

        sf::Vector2f InputManager::getVector(XinputGamepad::Vector vector);

        bool InputManager::isButtonDown(int gamepadIndex, XinputGamepad::Button gamepadButton);

        bool InputManager::isButtonPressed(int gamepadIndex, XinputGamepad::Button gamepadButton);

        bool InputManager::isButtonReleased(int gamepadIndex, XinputGamepad::Button gamepadButton);

        sf::Vector2f InputManager::getVector(int gamepadIndex, XinputGamepad::Vector vector);

        // Keyboard Vectors
        sf::Vector2f InputManager::getInputBindingVectorVector(int index);

        int InputManager::assignInputBindingVector(InputBindingVector* keyboardVector);

        int InputManager::reassignInputBindingVector(InputBindingVector* keyboardVector);

        // Button Bindings
        bool InputManager::isButtonDown(std::vector<InputBinding*> &bindings);

        bool InputManager::isButtonDown(InputBinding &binding);

        bool InputManager::isButtonPressed(std::vector<InputBinding*> &binding);

        bool InputManager::isButtonPressed(InputBinding &binding);

        bool InputManager::isButtonReleased(std::vector<InputBinding*> &binding);

        bool InputManager::isButtonReleased(InputBinding &binding);

        sf::Vector2f InputManager::getVector(std::vector<InputBinding*> &binding);

        sf::Vector2f InputManager::getVector(InputBinding &binding);

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