#include "XinputGamepad.hpp"
#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"

bit::XinputGamepad::XinputGamepad()
    : index(0), defaultDeadzone(.18f)
{
    updateControls();
}

bit::XinputGamepad::XinputGamepad(int _index)
    : index(_index), defaultDeadzone(.18f)
{
    updateControls();
}

bit::XinputGamepad::XinputGamepad(int _index, float _defaultDeadzone)
    : index(_index), defaultDeadzone(_defaultDeadzone)
{
    updateControls();
}

void bit::XinputGamepad::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    updateControls();
}

void bit::XinputGamepad::updateControls()
{
    // Start
    buttonCurrentDown[Button::Start] = sf::Joystick::isButtonPressed(index, 7);
	buttonLastDown[Button::Start] = buttonCurrentDown[Button::Start];

    // Back
    buttonCurrentDown[Button::Back] = sf::Joystick::isButtonPressed(index, 6);
	buttonLastDown[Button::Back] = buttonCurrentDown[Button::Back];

    // A
    buttonCurrentDown[Button::A] = sf::Joystick::isButtonPressed(index, 0);
	buttonLastDown[Button::A] = buttonCurrentDown[Button::A];

    // B
    buttonCurrentDown[Button::B] = sf::Joystick::isButtonPressed(index, 1);
	buttonLastDown[Button::B] = buttonCurrentDown[Button::B];

    // X
    buttonCurrentDown[Button::X] = sf::Joystick::isButtonPressed(index, 2);
	buttonLastDown[Button::X] = buttonCurrentDown[Button::X];

    // Y
    buttonCurrentDown[Button::Y] = sf::Joystick::isButtonPressed(index, 3);
	buttonLastDown[Button::Y] = buttonCurrentDown[Button::Y];

    // LeftBumper
    buttonCurrentDown[Button::LeftBumper] = sf::Joystick::isButtonPressed(index, 4);
	buttonLastDown[Button::LeftBumper] = buttonCurrentDown[Button::LeftBumper];

    // RightBumper
    buttonCurrentDown[Button::RightBumper] = sf::Joystick::isButtonPressed(index, 5);
	buttonLastDown[Button::RightBumper] = buttonCurrentDown[Button::RightBumper];

    // LeftJoystick
    buttonCurrentDown[Button::LeftJoystick] = sf::Joystick::isButtonPressed(index, 8);
	buttonLastDown[Button::LeftJoystick] = buttonCurrentDown[Button::LeftJoystick];

    // RightJoystick
    buttonCurrentDown[Button::RightJoystick] = sf::Joystick::isButtonPressed(index, 9);
	buttonLastDown[Button::RightJoystick] = buttonCurrentDown[Button::RightJoystick];

    // DpadUp
    buttonCurrentDown[Button::DpadUp] = sf::Joystick::getAxisPosition(index, sf::Joystick::PovX) == 100 ? true : false;
	buttonLastDown[Button::DpadUp] = buttonCurrentDown[Button::DpadUp];

    // DpadRight
    buttonCurrentDown[Button::DpadRight] = sf::Joystick::getAxisPosition(index, sf::Joystick::PovY) == 100 ? true : false;
	buttonLastDown[Button::DpadRight] = buttonCurrentDown[Button::DpadRight];

    // DpadDown
    buttonCurrentDown[Button::DpadDown] = sf::Joystick::getAxisPosition(index, sf::Joystick::PovX) == -100 ? true : false;
	buttonLastDown[Button::DpadDown] = buttonCurrentDown[Button::DpadDown];

    // DpadLeft
    buttonCurrentDown[Button::DpadLeft] = sf::Joystick::getAxisPosition(index, sf::Joystick::PovY) == -100 ? true : false;
	buttonLastDown[Button::DpadLeft] = buttonCurrentDown[Button::DpadLeft];

    // LeftTrigger
    float z = sf::Joystick::getAxisPosition(index, sf::Joystick::Z);
    buttonCurrentDown[Button::LeftTrigger] = z >= 25 ? true : false;
	buttonLastDown[Button::LeftTrigger] = buttonCurrentDown[Button::LeftTrigger];

    // RightTrigger
    buttonCurrentDown[Button::RightTrigger] = z <= -25 ? true : false;
	buttonLastDown[Button::RightTrigger] = buttonCurrentDown[Button::RightTrigger];

    float x = sf::Joystick::getAxisPosition(index, sf::Joystick::X);
    float y = sf::Joystick::getAxisPosition(index, sf::Joystick::Y);
    float r = sf::Joystick::getAxisPosition(index, sf::Joystick::R);
    float u = sf::Joystick::getAxisPosition(index, sf::Joystick::U);
    float px = sf::Joystick::getAxisPosition(index, sf::Joystick::PovX);
    float py = sf::Joystick::getAxisPosition(index, sf::Joystick::PovY);
    // V axis is ignored
    // Z axis is used for triggers

    float xPercentage = x > 0 ? x / 100 : x / -100;
    float yPercentage = y > 0 ? y / 100 : y / -100;
    float uPercentage = u > 0 ? u / 100 : r / -100;
    float rPercentage = r > 0 ? r / 100 : u / -100;

    sf::Vector2f xyv(x / 100, y / 100);
    sf::Vector2f urv(u / 100, r / 100);

    bit::VectorMath::applyDeadZone(&xyv, defaultDeadzone);
    bit::VectorMath::applyDeadZone(&urv, defaultDeadzone);

    xyv = bit::VectorMath::normalize(xyv);
    urv = bit::VectorMath::normalize(urv);

    xyv.x *= xPercentage;
    xyv.y *= yPercentage;
    urv.x *= uPercentage;
    urv.y *= rPercentage;

    axes[Vector::LeftJoystickVector] = xyv;
    axes[Vector::RightJoystickVector] = urv;

    // Up
    if(px > 90 && (py < 10 && py > -90))
    {
        axes[Vector::DpadVector].x = 0;
        axes[Vector::DpadVector].y = -1;
    }
    // Right Up
    else if(px > 60 && px < 80 && py > 60 && py < 80)
    {
        axes[Vector::DpadVector].x = 0.707f;
        axes[Vector::DpadVector].y = -0.707f;
    }
    // Right
    else if((px < 10 && px > -90) && py > 90)
    {
        axes[Vector::DpadVector].x = 1;
        axes[Vector::DpadVector].y = 0;
    }
    // Right Bottom
    else if(px > -80 && px < -60 && py > 60 && py < 80)
    {
        axes[Vector::DpadVector].x = 0.707f;
        axes[Vector::DpadVector].y = 0.707f;
    }
    // Bottom
    else if(px < -90 && (py < 10 && py > -90))
    {
        axes[Vector::DpadVector].x = 0;
        axes[Vector::DpadVector].y = 1;
    }
    // Left Bottom
    else if(px > -80 && px < -60 && py > -80 && py < -60)
    {
        axes[Vector::DpadVector].x = -0.707f;
        axes[Vector::DpadVector].y = 0.707f;
    }
    // Left
    else if((px < 10 && px > -90) && py < -90)
    {
        axes[Vector::DpadVector].x = -1;
        axes[Vector::DpadVector].y = 0;
    }
    // Left Top
    else if(px > 60 && px < 80 && py > -80 && py < -60)
    {
        axes[Vector::DpadVector].x = -0.707f;
        axes[Vector::DpadVector].y = -0.707f;
    }
}

sf::Vector2f bit::XinputGamepad::getVector(Vector vector)
{
    return axes[vector];
}

sf::Vector2f bit::XinputGamepad::getVector(Vector vector, float deadZone)
{
    if(deadZone > 0)
    {
        sf::Vector2f v = axes[vector];
        bit::VectorMath::applyDeadZone(&v, deadZone);
        return v;
    }

    return axes[vector];
}

bool bit::XinputGamepad::isPressed(Button button)
{
	return (!buttonLastDown[button] && buttonCurrentDown[button]);
}

bool bit::XinputGamepad::isDown(Button button)
{
	return buttonCurrentDown[button];
}

bool bit::XinputGamepad::isReleased(Button button)
{
	return (buttonLastDown[button] && !buttonCurrentDown[button]);
}