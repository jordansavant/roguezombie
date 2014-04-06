#pragma once
#ifndef BIT_XINPUTGAMEPAD_H
#define BIT_XINPUTGAMEPAD_H

#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"

namespace bit
{
    class XinputGamepad
    {
    public:
        XinputGamepad();

        XinputGamepad(int _index);

        XinputGamepad(int _index, float _defaultDeadzone);

        int index;
        int isConnected;
        float defaultDeadzone;

        enum Button
        {
            Start,
            Back,
            A,
            B,
            X,
            Y,
            DpadUp,
            DpadRight,
            DpadDown,
            DpadLeft,
            LeftBumper,
            RightBumper,
            LeftTrigger,
            RightTrigger,
            LeftJoystick,
            RightJoystick
        };

        enum Vector
        {
            LeftJoystickVector,
            RightJoystickVector,
            LeftTriggerVector,
            RightTriggerVector,
            DpadVector,
        };

        void update(sf::Time &gameTime);

        void updateControls();

        sf::Vector2f getVector(Vector vector);

        sf::Vector2f getVector(Vector vector, float deadZone);

        bool isPressed(Button button);

        bool isDown(Button button);

        bool isReleased(Button button);

    private:
        bool buttonLastDown[16];
		bool buttonCurrentDown[16];
        sf::Vector2f axes[5];
    };
}

#endif