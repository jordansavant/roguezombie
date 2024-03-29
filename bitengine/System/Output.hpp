#pragma once
#ifndef BIT_OUTPUT_H
#define BIT_OUTPUT_H

#include "PlatformDefinitions.hpp"
#include <string>
#include <sstream>

namespace bit
{
    class Output
    {
    public:

        static void Debug(unsigned int integer);

        static void Debug(int integer);

        static void Debug(float floatVal);

        static void Debug(std::string message);

        static std::stringstream stream;

        static void Flush();
    };
}

#endif