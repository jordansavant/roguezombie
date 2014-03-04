#pragma once
#ifndef BIT_OUTPUT_H
#define BIT_OUTPUT_H

#include "PlatformDefinitions.hpp"
#include <string>

namespace bit
{
    class Output
    {
    public:

        static void Debug(int integer);

        static void Debug(std::string message);
    };
}

#endif