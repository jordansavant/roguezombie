#pragma once
#ifndef BIT_RANDOM_H
#define BIT_RANDOM_H

#include "mtrand.hpp"

#define BIT_RANDOM_MAX 4294967295 // unsigned int 32 (2147483647 = int 32)

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

        unsigned int next(unsigned int max);

        unsigned int next(unsigned int min, unsigned int max);
        
        int of(int a, int b);
        int of(int a, int b, int c);
        int of(int a, int b, int c, int d);
        int of(int a, int b, int c, int d, int e);
        int of(int a, int b, int c, int d, int e, int f);

        float nextFloat();

    private:

        MTRand_int32 engine;
    };
}

#endif