#include "InputManager.hpp"
#include "SFML/Graphics.hpp"
#include "../Game/Game.hpp"
#include "../Game/GameComponent.hpp"
#include "XinputGamepad.hpp"
#include "InputBinding.hpp"
#include "InputBindingVector.hpp"
#include "InputTextListener.hpp"
#include "../System/Output.hpp"

bit::InputManager::InputManager(bit::Game* _game)
    : bit::GameComponent(_game)
{
	// Mouse
	for ( int i = sf::Mouse::Button::Left; i < sf::Mouse::Button::XButton2; i++ )
	{
		mouseButtonCurrentDown[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
		mouseButtonLastDown[i] = mouseButtonCurrentDown[i];
	}

	// Keyboard
	for ( int i = sf::Keyboard::Key::A; i < sf::Keyboard::Key::Pause; i++ )
	{
		keyboardKeyCurrentDown[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
		keyboardKeyLastDown[i] = keyboardKeyCurrentDown[i];
	}

    // Gamepad
    gamepadConnectionCounter = 0;
    for ( int i = 0; i < gamepadCount; i++)
    {
        gamepads[i].isConnected = sf::Joystick::isConnected(i);
    }
}

bit::InputManager::~InputManager()
{
    while(!inputBindingVectors.empty()) delete inputBindingVectors.back(), inputBindingVectors.pop_back();

	for (unsigned int i = 0; i < inputTextListeners.size(); i++)
	{
		delete inputTextListeners[i];
	}
}

void bit::InputManager::preinspectEvents()
{
    mouseWheelCurrentDelta = 0;
}

void bit::InputManager::inspectMouseWheelEvent(sf::Event e)
{
    // Mouse Wheel
    if(e.type == sf::Event::MouseWheelMoved)
    {
        mouseWheelCurrentDelta = e.mouseWheel.delta;
    }
}

void bit::InputManager::inspectTextEnteredEvent(sf::Event e)
{
	// Handle ASCII characters only
	if (e.text.unicode < 128)
	{
		for (unsigned int i = 0; i < inputTextListeners.size(); i++)
		{
			InputTextListener* itl = inputTextListeners[i];
			if (itl->listen == false)
				continue;

			switch (static_cast<unsigned int>(e.text.unicode))
			{
				case 13: // return
					itl->isEntered = true;
					break;
				case 8: // delete
					itl->input = itl->input.substr(0, itl->input.size() - 1);
					itl->isChanged = true;
					break;
				default:
					// character range
					if ((e.text.unicode >= 32 && e.text.unicode <= 126))
					{
						char input;
						input = static_cast<char>(e.text.unicode);
						if (itl->ignoreCharacters.find(input) == itl->ignoreCharacters.npos)
							itl->input += input;
					}
					itl->isChanged = true;
					break;
			}
		}
	}
}

bit::InputTextListener* bit::InputManager::getInputTextListener()
{
	InputTextListener* i = new InputTextListener();
	inputTextListeners.push_back(i);
	return i;
}

void bit::InputManager::update(sf::Time &gameTime)
{
	// Mouse
	for ( int i = sf::Mouse::Button::Left; i < sf::Mouse::Button::XButton2; i++ )
	{
		mouseButtonLastDown[i] = mouseButtonCurrentDown[i];
		mouseButtonCurrentDown[i] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));
	}

	// Keyboard
	for ( int i = sf::Keyboard::Key::A; i < sf::Keyboard::Key::Pause; i++ )
	{
		keyboardKeyLastDown[i] = keyboardKeyCurrentDown[i];
		keyboardKeyCurrentDown[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}

    // Gamepad
    gamepads[gamepadConnectionCounter].isConnected = sf::Joystick::isConnected(gamepadConnectionCounter);
    gamepadConnectionCounter++;
    gamepadConnectionCounter = gamepadConnectionCounter > 7 ? 0 : gamepadConnectionCounter;
    for ( int i = 0; i < gamepadCount; i++)
    {
        if(gamepads[i].isConnected)
        {
            gamepads[i].update(gameTime);
        }
    }

    // InputBindingVectors
    for(unsigned int i = 0; i < inputBindingVectors.size(); i++)
    {
        inputBindingVectors[i]->vector.x = 0;
        inputBindingVectors[i]->vector.y = 0;
        if (isButtonDown(inputBindingVectors[i]->up))
        {
            inputBindingVectors[i]->vector.y = -1;
        }
        if (isButtonDown(inputBindingVectors[i]->right))
        {
            inputBindingVectors[i]->vector.x = 1;
        }
        if (isButtonDown(inputBindingVectors[i]->down))
        {
            inputBindingVectors[i]->vector.y = 1;
        }
        if (isButtonDown(inputBindingVectors[i]->left))
        {
            inputBindingVectors[i]->vector.x = -1;
        }

        bit::VectorMath::normalize(inputBindingVectors[i]->vector);
    }
}

// Mouse
bool bit::InputManager::isButtonDown(sf::Mouse::Button mouseButton)
{
	return mouseButtonCurrentDown[mouseButton];
}

bool bit::InputManager::isButtonPressed(sf::Mouse::Button mouseButton)
{
	return (!mouseButtonLastDown[mouseButton] && mouseButtonCurrentDown[mouseButton]);
}

bool bit::InputManager::isButtonReleased(sf::Mouse::Button mouseButton)
{
	return (mouseButtonLastDown[mouseButton] && !mouseButtonCurrentDown[mouseButton]);
}

// Keyboard
bool bit::InputManager::isButtonDown(sf::Keyboard::Key keyboardKey)
{
	return keyboardKeyCurrentDown[keyboardKey];
}

bool bit::InputManager::isButtonPressed(sf::Keyboard::Key keyboardKey)
{
	return (!keyboardKeyLastDown[keyboardKey] && keyboardKeyCurrentDown[keyboardKey]);
}

bool bit::InputManager::isButtonReleased(sf::Keyboard::Key keyboardKey)
{
	return (keyboardKeyLastDown[keyboardKey] && !keyboardKeyCurrentDown[keyboardKey]);
}

// Gamepad
bool bit::InputManager::isButtonDown(XinputGamepad::Button gamepadButton)
{
    for ( int i = 0; i < gamepadCount; i++)
    {
        if(gamepads[i].isConnected && gamepads[i].isDown(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

bool bit::InputManager::isButtonPressed(XinputGamepad::Button gamepadButton)
{
	for ( int i = 0; i < gamepadCount; i++)
    {
        if(gamepads[i].isConnected && gamepads[i].isPressed(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

bool bit::InputManager::isButtonReleased(XinputGamepad::Button gamepadButton)
{
	for ( int i = 0; i < gamepadCount; i++)
    {
        if(gamepads[i].isConnected && gamepads[i].isReleased(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

int bit::InputManager::getMouseWheelTicks()
{
    return mouseWheelCurrentDelta;
}

sf::Vector2f bit::InputManager::getVector(XinputGamepad::Vector vector)
{
    sf::Vector2f v(0, 0);
    for ( int i = 0; i < gamepadCount; i++)
    {
        if(gamepads[i].isConnected)
        {
            v = gamepads[i].getVector(vector);
            if(v.x != 0 || v.y != 0)
            {
                return v;
            }
        }
    }

    v.x = 0;
    v.y = 0;
    return v;
}

bool bit::InputManager::isButtonDown(int gamepadIndex, XinputGamepad::Button gamepadButton)
{
    if(gamepadIndex > 0 && gamepadIndex < gamepadCount)
    {
        if(gamepads[gamepadIndex].isConnected && gamepads[gamepadIndex].isDown(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

bool bit::InputManager::isButtonPressed(int gamepadIndex, XinputGamepad::Button gamepadButton)
{
    if(gamepadIndex > 0 && gamepadIndex < gamepadCount)
    {
        if(gamepads[gamepadIndex].isConnected && gamepads[gamepadIndex].isPressed(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

bool bit::InputManager::isButtonReleased(int gamepadIndex, XinputGamepad::Button gamepadButton)
{
    if(gamepadIndex > 0 && gamepadIndex < gamepadCount)
    {
        if(gamepads[gamepadIndex].isConnected && gamepads[gamepadIndex].isReleased(gamepadButton))
        {
            return true;
        }
    }

    return false;
}

sf::Vector2f bit::InputManager::getVector(int gamepadIndex, XinputGamepad::Vector vector)
{
    if(gamepadIndex > 0 && gamepadIndex < gamepadCount)
    {
        if(gamepads[gamepadIndex].isConnected)
        {
            return gamepads[gamepadIndex].getVector(vector);
        }
    }

    return sf::Vector2f(0, 0);
}

// Keyboard Vectors
sf::Vector2f bit::InputManager::getInputBindingVectorVector(unsigned int index)
{
    if(index < inputBindingVectors.size())
    {
        return inputBindingVectors[index]->vector;
    }

    return sf::Vector2f(0, 0);
}

int bit::InputManager::assignInputBindingVector(InputBindingVector* keyboardVector)
{
    inputBindingVectors.push_back(keyboardVector);
    return inputBindingVectors.size() - 1;
}

int bit::InputManager::reassignInputBindingVector(InputBindingVector* keyboardVector)
{
    inputBindingVectors.push_back(keyboardVector);
    return inputBindingVectors.size() - 1;
}

// Button Bindings
bool bit::InputManager::isButtonDown(std::vector<InputBinding*> &bindings)
{
    for(unsigned int i = 0; i < bindings.size(); i++)
    {
        if(isButtonDown(*bindings[i]))
        {
            return true;
        }
    }

    return false;
}
bool bit::InputManager::isButtonDown(InputBinding &binding)
{
    switch(binding.inputType)
    {
        case InputBinding::InputType::MouseButton:
            return isButtonDown(static_cast<sf::Mouse::Button>(binding.inputEnum));
        case InputBinding::InputType::KeyboardButton:
            return isButtonDown(static_cast<sf::Keyboard::Key>(binding.inputEnum));
        case InputBinding::InputType::GamepadButton:
            return isButtonDown(static_cast<XinputGamepad::Button>(binding.inputEnum));
        case InputBinding::InputType::GamepadVector:
        case InputBinding::InputType::InputBindingVector:
            break;
    }

    return false;
}

bool bit::InputManager::isButtonPressed(std::vector<InputBinding*> &binding)
{
    for(unsigned int i = 0; i < binding.size(); i++)
    {
        bool r = false;
        switch(binding[i]->inputType)
        {
            case InputBinding::InputType::MouseButton:
                return isButtonPressed(static_cast<sf::Mouse::Button>(binding[i]->inputEnum));
            case InputBinding::InputType::KeyboardButton:
                return isButtonPressed(static_cast<sf::Keyboard::Key>(binding[i]->inputEnum));
            case InputBinding::InputType::GamepadButton:
                return isButtonPressed(static_cast<XinputGamepad::Button>(binding[i]->inputEnum));
            case InputBinding::InputType::GamepadVector:
            case InputBinding::InputType::InputBindingVector:
                break;
        }

        if(r)
        {
            return r;
        }
    }

    return false;
}
bool bit::InputManager::isButtonPressed(InputBinding &binding)
{
    switch(binding.inputType)
    {
        case InputBinding::InputType::MouseButton:
            return isButtonPressed(static_cast<sf::Mouse::Button>(binding.inputEnum));
        case InputBinding::InputType::KeyboardButton:
            return isButtonPressed(static_cast<sf::Keyboard::Key>(binding.inputEnum));
        case InputBinding::InputType::GamepadButton:
            return isButtonPressed(static_cast<XinputGamepad::Button>(binding.inputEnum));
        case InputBinding::InputType::GamepadVector:
        case InputBinding::InputType::InputBindingVector:
            break;
    }

    return false;
}

bool bit::InputManager::isButtonReleased(std::vector<InputBinding*> &binding)
{
    for(unsigned int i = 0; i < binding.size(); i++)
    {
        bool r = false;
        switch(binding[i]->inputType)
        {
            case InputBinding::InputType::MouseButton:
                r = isButtonReleased(static_cast<sf::Mouse::Button>(binding[i]->inputEnum));
                break;
            case InputBinding::InputType::KeyboardButton:
                r = isButtonReleased(static_cast<sf::Keyboard::Key>(binding[i]->inputEnum));
                break;
            case InputBinding::InputType::GamepadButton:
                r = isButtonReleased(static_cast<XinputGamepad::Button>(binding[i]->inputEnum));
                break;
            case InputBinding::InputType::GamepadVector:
            case InputBinding::InputType::InputBindingVector:
                break;
        }

        if(r)
        {
            return r;
        }
    }

    return false;
}

bool bit::InputManager::isButtonReleased(InputBinding &binding)
{
    switch(binding.inputType)
    {
        case InputBinding::InputType::MouseButton:
            return isButtonReleased(static_cast<sf::Mouse::Button>(binding.inputEnum));
        case InputBinding::InputType::KeyboardButton:
            return isButtonReleased(static_cast<sf::Keyboard::Key>(binding.inputEnum));
        case InputBinding::InputType::GamepadButton:
            return isButtonReleased(static_cast<XinputGamepad::Button>(binding.inputEnum));
        case InputBinding::InputType::GamepadVector:
        case InputBinding::InputType::InputBindingVector:
            break;
    }

    return false;
}

sf::Vector2f bit::InputManager::getVector(std::vector<InputBinding*> &binding)
{
    sf::Vector2f v(0, 0);
    for(unsigned int i = 0; i < binding.size(); i++)
    {
        switch(binding[i]->inputType)
        {
            case InputBinding::InputType::GamepadVector:
                v = getVector(static_cast<XinputGamepad::Vector>(static_cast<sf::Mouse::Button>(binding[i]->inputEnum)));
                break;
            case InputBinding::InputType::InputBindingVector:
                v = getInputBindingVectorVector(binding[i]->inputEnum);
                break;
            case InputBinding::InputType::MouseButton:
            case InputBinding::InputType::KeyboardButton:
            case InputBinding::InputType::GamepadButton:
                break;
        }

        if(v.x != 0 || v.y != 0)
        {
            return v;
        }
    }

    return v;
}

sf::Vector2f bit::InputManager::getVector(InputBinding &binding)
{
    switch(binding.inputType)
    {
        case InputBinding::InputType::GamepadVector:
            return getVector(static_cast<XinputGamepad::Vector>(static_cast<sf::Mouse::Button>(binding.inputEnum)));
        case InputBinding::InputType::InputBindingVector:
            return getInputBindingVectorVector(binding.inputEnum);
        case InputBinding::InputType::MouseButton:
        case InputBinding::InputType::KeyboardButton:
        case InputBinding::InputType::GamepadButton:
            break;
    }

    return sf::Vector2f(0, 0);
}