#include "Requirement.hpp"
#include "../Character.hpp"

Requirement::Requirement()
{
}

bool Requirement::checkFullfilled(Character* c)
{
    isFullfilled = check(c);
    return isFullfilled;
}