#pragma once
#ifndef RZ_DIALOGRESPONSE_H
#define RZ_DIALOGRESPONSE_H

#include "Dialog.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Network.hpp"
#include <string>
#include <functional>

class DialogNode;
class Body;

class DialogResponse
{
public:

    DialogResponse();

    Dialog response;
    DialogNode* next;
    std::function<void(Body* speaker, Body* listener)> onChoose;
    std::function<bool(Body* speaker, Body* listener)> meetsRequirements;

    void choose(Body* speaker, Body* listener);
};

#endif