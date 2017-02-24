#pragma once
#ifndef RZ_INVENTORYITEMLABEL_H
#define RZ_INVENTORYITEMLABEL_H

#include "Inventory.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"
#include "../../server/Character.hpp"

class InventoryIcon;
class InventoryEquipmentSlot;
class InventoryPositionSlot;
class InventoryLootSlot;
class ActionBarSlot;

class InventoryItemLabel : public bit::Element
{
public:

    InventoryItemLabel(Hud* hud, Item::Schema& itemSchema, float relativeX, float relativeY, AnchorType anchorType);

    virtual ~InventoryItemLabel();

    Hud* hud;
    Item::Schema itemSchema;
    InventoryIcon* icon;
    InventoryEquipmentSlot* currentEquipmentSlot;
    InventoryPositionSlot* currentPositionSlot;
    InventoryLootSlot* currentLootSlot;
    ActionBarSlot* currentActionSlot;
    std::string tooltipTag;

    bool canIssueItemCommand();

    void dropResult(bool result);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    bool dropOntoEquipmentSlot(InventoryEquipmentSlot* slot);

    bool dropOntoInventorySlot(InventoryPositionSlot* slot);

    bool dropOntoLootSlot(InventoryLootSlot* slot);

    bool dropOntoActionSlot(ActionBarSlot* slot);

    void sendClientRequest_EquipItemFromInventoryToSlot(Character::EquipmentSlot equipmentSlot);

    void sendClientRequest_SwapEquipmentBySlot(Character::EquipmentSlot equipmentSlotA, Character::EquipmentSlot equipmentSlotB);

    void sendClientRequest_EquipItemFromLootToSlot(Character::EquipmentSlot equipmentSlot);

    void sendClientRequest_MoveInventoryItemToPosition(unsigned int position);

    void sendClientRequest_MoveEquippedItemToInventoryPosition(Character::EquipmentSlot equipmentSlot, unsigned int position);

    void sendClientRequest_MoveLootItemToInventoryPosition(unsigned int position);

    void sendClientRequest_MoveLootItemToLootPosition(unsigned int position);

    void sendClientRequest_MoveEquippedItemToLootPosition(Character::EquipmentSlot equipmentSlot, unsigned int position);

    void sendClientRequest_MoveInventoryItemToLootPosition(unsigned int position);

};

#endif