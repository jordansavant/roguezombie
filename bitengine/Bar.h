#pragma once
#ifndef BIT_BAR_H
#define BIT_BAR_H

#include <functional>

namespace bit
{
    class Bar
    {
    public:
        void foo();

        bool lamb(std::function<bool (int)> func);
    };
}

#endif
