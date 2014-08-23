#pragma once
#ifndef RZ_SERVEREVENT_H
#define RZ_SERVEREVENT_H

enum ServerEvent
{
    LeavingLevel,
    ArrivedLevel,

    SpectateBegin,
    SpectateEnd,

    MissionAssigned,
    MissionCompleted,
    
    ItemAdded,
    ItemRemoved,
    
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