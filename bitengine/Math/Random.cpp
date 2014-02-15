#include "Random.hpp"
#include <random>
#include "mtrand.hpp"

bit::Random::Random()
{
    engine.seed(1);
}

bit::Random::Random(unsigned int _seed)
{
    engine.seed(_seed);
}

unsigned int bit::Random::min()
{
    return 0;
    //return engine.min();
}

unsigned int bit::Random::max()
{
    return RAND_MAX;
    //return engine.max();
}

unsigned int bit::Random::next()
{
    return engine();
}

float bit::Random::nextFloat()
{
    return (float)engine() / (float)RAND_MAX;
}
