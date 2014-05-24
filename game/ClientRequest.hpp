#pragma once
#ifndef RZ_CLIENTREQUEST_H
#define RZ_CLIENTREQUEST_H

enum ClientRequest
{
    GetInteractionOptions,
    GetCharacterStatistics,
    Interaction,
    DialogResponse,
    TransferItem,
    CloseInventory,
    EquipItem,
    UnequipItem,
};

#endif