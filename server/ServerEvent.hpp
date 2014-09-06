#pragma once
#ifndef RZ_SERVEREVENT_H
#define RZ_SERVEREVENT_H

enum ServerEvent
{
    LeavingLevel,
    ArrivedLevel,
    CannotTransitionInCombat,

    SpectateBegin,
    SpectateEnd,

    MissionAssigned,
    MissionCompleted,
    
    InventoryUpdate,
    ItemAdded,
    ItemUpdated,
    ItemRemoved,
    EquipmentAdded,
    EquipmentUpdated,
    EquipmentRemoved,
    EquipmentSwapped,
    
    CombatTurnStart,
    CombatTurnEnd,
    CombatDecisionStart,
    CombatDecisionEnd,

    CharacterAttacksPlayer,
    CharacterMissesPlayer,
    PlayerAttacksCharacter,
    PlayerMissesCharacter,

    GameVictory,
    GameDefeat,
};

#endif