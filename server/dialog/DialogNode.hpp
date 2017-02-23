#pragma once
#ifndef RZ_DIALOGNODE_H
#define RZ_DIALOGNODE_H

#include "Dialog.hpp"
#include "DialogEntry.hpp"
#include "DialogResponse.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Network.hpp"
#include <string>
#include <functional>

class Body;

class DialogNode
{
public:

    DialogNode();

    virtual ~DialogNode();

    bool pendingDelete;
    Dialog prompt;
    std::vector<DialogResponse> responses;

    void addResponse(Dialog responseNode, DialogNode* next = NULL, std::function<bool(Body* speaker, Body* listener)> meetsRequirements = NULL, std::function<void(Body* speaker, Body* listener)> onChoose = NULL);

    DialogNode* chooseResponse(Body* speaker, Body* listener, unsigned int responseId);
    
    virtual void prepareSnapshot(bit::ServerPacket &packet);

};

#endif