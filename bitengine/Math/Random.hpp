#pragma once
#ifndef BIT_RANDOM_H
#define BIT_RANDOM_H

#include "mtrand.hpp"

#define BIT_RANDOM_MAX 2147483647 // int 32

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

        MTRand_int32 engine;
    };
}

#endif