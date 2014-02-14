#include "Random.hpp"
#include <random>
#include <chrono>

bit::Random::Random()
{
    unsigned _seed = std::chrono::system_clock::now().time_since_epoch().count();
    seed(_seed);
}

bit::Random::Random(unsigned int _seed)
{
    seed(_seed);
}

void bit::Random::seed(unsigned int _seed)
{
    engine.seed(_seed);
}

unsigned int bit::Random::min()
{
    return engine.min();
}

unsigned int bit::Random::max()
{
    return engine.max();
}

unsigned int bit::Random::next()
{
    return engine();
}

float bit::Random::nextFloat()
{
    return (float)engine() / (float)max();
}
