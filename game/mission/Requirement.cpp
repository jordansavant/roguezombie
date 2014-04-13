#include "Requirement.hpp"
#include "../Character.hpp"

Requirement::Requirement()
    : check(NULL), isFullfilled(false), generationType(GenerationType::Scripted)
{
}

bool Requirement::checkFullfilled(Character* c)
{
    isFullfilled = check(c);
    return isFullfilled;
}