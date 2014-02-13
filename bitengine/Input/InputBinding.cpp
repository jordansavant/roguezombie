#include "InputBinding.hpp"

bit::InputBinding::InputBinding()
    : inputType(InputType::MouseButton), inputEnum(-1), gamepadIndex(-1)
{
}

bit::InputBinding::InputBinding(InputType _inputType, int _inputEnum)
    : inputType(_inputType), inputEnum(_inputEnum), gamepadIndex(-1)
{
}

bit::InputBinding::InputBinding(InputType _inputType, int _inputEnum, int _gamepadIndex)
    : inputType(_inputType), inputEnum(_inputEnum), gamepadIndex(_gamepadIndex)
{
}