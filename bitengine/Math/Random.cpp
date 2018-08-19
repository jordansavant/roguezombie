#include "Random.hpp"
#include <random>
#include <cstdarg>
#include "mtrand.hpp"

bit::Random::Random()
{
    engine.seed(std::rand());
}

bit::Random::Random(unsigned int _seed)
{
    engine.seed(_seed);
}

void bit::Random::seed(unsigned int _seed)
{
    engine.seed(_seed);
}

unsigned int bit::Random::min()
{
    return 0;
}

unsigned int bit::Random::max()
{
    return BIT_RANDOM_MAX;
}

unsigned int bit::Random::next()
{
    return engine();
}

unsigned int bit::Random::next(unsigned int max)
{
    return next() % max;
}

unsigned int bit::Random::next(unsigned int min, unsigned int max)
{
    return next() % (max - min) + min;
}



int bit::Random::of(int a, int b)
{
    switch (next(2)) {
        case 0: return a;
        case 1: return b;
    }
}
int bit::Random::of(int a, int b, int c)
{
    switch (next(3)) {
        case 0: return a;
        case 1: return b;
        case 2: return c;
    }
}
int bit::Random::of(int a, int b, int c, int d)
{
    switch (next(4)) {
        case 0: return a;
        case 1: return b;
        case 2: return c;
        case 3: return d;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e)
{
    switch (next(5)) {
        case 0: return a;
        case 1: return b;
        case 2: return c;
        case 3: return d;
        case 4: return e;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f)
{
    switch (next(6)) {
        case 0: return a;
        case 1: return b;
        case 2: return c;
        case 3: return d;
        case 4: return e;
        case 5: return f;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f, int g)
{
    switch (next(7)) {
    case 0: return a;
    case 1: return b;
    case 2: return c;
    case 3: return d;
    case 4: return e;
    case 5: return f;
    case 6: return g;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f, int g, int h)
{
    switch (next(8)) {
    case 0: return a;
    case 1: return b;
    case 2: return c;
    case 3: return d;
    case 4: return e;
    case 5: return f;
    case 6: return g;
    case 7: return h;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
    switch (next(9)) {
    case 0: return a;
    case 1: return b;
    case 2: return c;
    case 3: return d;
    case 4: return e;
    case 5: return f;
    case 6: return g;
    case 7: return h;
    case 8: return i;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    switch (next(10)) {
    case 0: return a;
    case 1: return b;
    case 2: return c;
    case 3: return d;
    case 4: return e;
    case 5: return f;
    case 6: return g;
    case 7: return h;
    case 8: return i;
    case 9: return j;
    }
}
int bit::Random::of(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k)
{
    switch (next(11)) {
    case 0: return a;
    case 1: return b;
    case 2: return c;
    case 3: return d;
    case 4: return e;
    case 5: return f;
    case 6: return g;
    case 7: return h;
    case 8: return i;
    case 9: return j;
    case 10: return k;
    }
}

float bit::Random::nextFloat()
{
    return (float)engine() / (float)max();
}