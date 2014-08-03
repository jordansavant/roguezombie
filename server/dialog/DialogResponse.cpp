#include "DialogResponse.hpp"

DialogResponse::DialogResponse()
    : response(), next(NULL), onChoose(NULL), meetsRequirements(NULL)
{
}

void DialogResponse::choose(Body* speaker, Body* listener)
{
    if(onChoose)
    {
        onChoose(speaker, listener);
    }
}
