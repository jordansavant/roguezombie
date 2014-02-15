#pragma once
#ifndef BIT_RANDOM_H
#define BIT_RANDOM_H

#include "mtrand.hpp"

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

        //std::mt19937 engine;
        MTRand_int32 engine;
    };
}

#endif
