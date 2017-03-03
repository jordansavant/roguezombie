#include "Random.hpp"
#include <random>
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

unsigned int bit::Random::next(int max)
{
    return next() % max;
}

unsigned int bit::Random::next(int min, int max)
{
    return next() % (max - min) + min;
}

float bit::Random::nextFloat()
{
    return (float)engine() / (float)max();
}