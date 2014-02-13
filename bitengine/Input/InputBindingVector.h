#pragma once
#ifndef BIT_INPUTBINDINGVECTOR_H
#define BIT_INPUTBINDINGVECTOR_H

#include "SFML/System.hpp"
#include "InputBinding.h"

namespace bit
{
    class InputBindingVector
    {
    public:
        InputBindingVector();

        InputBindingVector(InputBinding _up, InputBinding _right, InputBinding _down, InputBinding _left);

        InputBinding up;
        InputBinding right;
        InputBinding down;
        InputBinding left;
        sf::Vector2f vector;
    };
}

#endif