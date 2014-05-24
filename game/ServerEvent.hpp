#pragma once
#ifndef RZ_SERVEREVENT_H
#define RZ_SERVEREVENT_H

enum ServerEvent
{
    MissionAssigned,
    MissionCompleted,
    ItemAdded,
    ItemRemoved,
    CombatTurnStart,
    CombatTurnEnd,
    CombatDecisionStart,
    CombatDecisionEnd
};

#endif