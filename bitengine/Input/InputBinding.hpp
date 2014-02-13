#pragma once
#ifndef BIT_INPUTBINDING_H
#define BIT_INPUTBINDING_H

namespace bit
{
    class InputBinding
    {
    public:
        enum InputType
        {
            MouseButton,
            KeyboardButton,
            GamepadButton,
            GamepadVector,
            InputBindingVector
        };

        InputBinding();

        InputBinding(InputType _inputType, int _inputEnum);

        InputBinding(InputType _inputType, int _inputEnum, int _gamepadIndex);

        InputType inputType;
        int inputEnum;
        int gamepadIndex;
    };
}

#endif