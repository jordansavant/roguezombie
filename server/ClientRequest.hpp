#pragma once
#ifndef RZ_CLIENTREQUEST_H
#define RZ_CLIENTREQUEST_H

/*
 * Server Request Response system
 */
enum ClientRequest
{
    ////////////////////////////////////////////////
    ////////// Interaction Based Requests //////////
    ////////////////////////////////////////////////

    // Takes a tile and builds the interaction option
	// as a response for available options
	// If there is no tile, or no options the response is false
    GetAvailableInteractionsForBodyOnTile,

    // Takes a tile ID and an Interaction enum and if the tile has a body, tells that body
	// to handle the interaction
	// See: Interactions
    ProcessInteractionForBodyOnTile,
    

    ////////////////////////////////////////////////
    //////////// Dialog Based Requests /////////////
    ////////////////////////////////////////////////

    // Prereq: needs a dialog transaction between the player and a character
	// Takes the dialog and lets the other party handle it
	// Response is the new dialog tree
    RespondToDialog,
    

    ////////////////////////////////////////////////
    ////////// Loot Transaction Requests ///////////
    ////////////////////////////////////////////////
    
    // Prereq: requires an inventory transaction
	// Finds the item in the counterparty inventory
	// If it is found it removes it and and moves it to the player
	// If it is not found or there is no transaction it returns false
    TransferItemFromCounterpartyToParty,

    // Prereq: requires an inventory transaction
	// Finds the item in the player inventory
	// If it is found it removes it and and moves it to the counterparty
	// If it is not found or there is no transaction it returns false
    // NOT YET IMPLEMENTED
    TransferItemFromPartyToCounterparty,

    // Prereq: requires an inventory transaction
	// If the player character has an counterparty it nullifies the transaction
	// Return is constantly void
    CloseItemTransaction,

    
    ////////////////////////////////////////////////
    ////////////// Inventory Requests //////////////
    ////////////////////////////////////////////////

    // Takes an item id, and if found in the inventory attempts to equip it in the slot
	// If the slot is not available or if the item does not match the criteria it fails
	// Return is constantly void
    EquipItemFromInventoryToSlot,

    // Takes a slot and removes any item from it placing it in the inventory
	// If no slotted item exists it is ignored
	// Return is constantly void
    UnequipItemFromSlotToInventory,
};

#endif