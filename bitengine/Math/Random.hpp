#pragma once
#ifndef BIT_MATHHELPER_H
#define BIT_MATHHELPER_H

#include <random>

namespace bit
{
    class Random
    {
    public:

        Random();

        Random(unsigned int seed);

        void seed(unsigned int seed);

        unsigned int max();

        unsigned int min();

        unsigned int next();

        float nextFloat();

    private:

        std::mt19937 engine;
    };
}

#endif
