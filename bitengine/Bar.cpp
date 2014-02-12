#include "Bar.h"

void bit::Bar::foo()
{
    return;
}

bool bit::Bar::lamb(std::function<bool (int)> func)
{
    return func(10);
}
