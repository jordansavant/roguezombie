#pragma once
#ifndef RZ_CLIENTREQUEST_H
#define RZ_CLIENTREQUEST_H

enum ClientRequest
{
    GetInteractionOptions,
    Interaction,
    DialogResponse,
    TransferItem,
    CloseInventory,
    EquipItem,
    UnequipItem,
};

#endif