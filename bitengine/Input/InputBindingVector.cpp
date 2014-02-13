#include "InputBindingVector.hpp"
#include "InputBinding.hpp"

bit::InputBindingVector::InputBindingVector()
{
}

bit::InputBindingVector::InputBindingVector(InputBinding _up, InputBinding _right, InputBinding _down, InputBinding _left)
: up(_up), right(_right), down(_down), left(_left)
{
}