#include "DialogNode.hpp"
#include "../Body.hpp"

DialogNode::DialogNode()
    : pendingDelete(false), prompt()
{
}

DialogNode::~DialogNode()
{
    // Recurse through responses deleting all nodes
    pendingDelete = true;
    for(unsigned int i=0; i < responses.size(); i++)
    {
        DialogResponse* response = &responses[i];
        if(response->next && !response->next->pendingDelete)
        {
            delete response->next;
            response->next = NULL;
        }
    }
}

void DialogNode::addResponse(Dialog responseDialog, DialogNode* next, std::function<bool(Body* speaker, Body* listener)> meetsRequirements, std::function<void(Body* speaker, Body* listener)> onChoose)
{
    DialogResponse dr;
    dr.response = responseDialog;
    dr.next = next;
    dr.onChoose = onChoose;
    dr.meetsRequirements = meetsRequirements;
    responses.push_back(dr);
}


DialogNode* DialogNode::chooseResponse(Body* speaker, Body* listener, unsigned int responseId)
{
    for(unsigned int i=0; i < responses.size(); i++)
    {
        DialogResponse* response = &responses[i];

        if(response->response.id == responseId)
        {
            response->choose(speaker, listener);
                
            return response->next;
        }
    }
        
    return NULL;
}

void DialogNode::prepareSnapshot(bit::ServerPacket &packet)
{
    // TODO, dont pack responses for requirement failure etc.
}
