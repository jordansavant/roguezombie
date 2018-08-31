#include "RpgSystem.hpp"
#include "Character.hpp"
#include "Tile.hpp"
#include "Level.hpp"
#include "items/Item.hpp"
#include "../bitengine/Math.hpp"

int RpgSystem::masterIntelligence = 2;
int RpgSystem::masterSpeed = 3;
int RpgSystem::masterDexterity = 2;
int RpgSystem::masterStrength = 2;

// DEXTERITY FACTOR
// Brief: Floating point representing current dexterity against the target master dexterity
// CurrentDexterity / MasterDexterity
// Can result in 0-1 values for most, exceeding 1 for masters
float RpgSystem::calculateDexterityFactor(Character* character)
{
    return (float)character->schema.dexterity / (float)masterDexterity;
}

// STRENGTH FACTOR
// Brief: Floating point representing current strength against the target master strength
// CurrentStrength / MasterStrength
// Can result in 0-1 values for most, exceeding 1 for masters
float RpgSystem::calculateStrengthFactor(Character* character)
{
    return (float)character->schema.strength / (float)masterStrength;
}

// SPEED FACTOR
// Brief: Floating point representing current speed against the target master speed
// CurrentSpeed / MasterSpeed
// Can result in 0-1 values for most, exceeding 1 for masters
float RpgSystem::calculateSpeedFactor(Character* character)
{
    return (float)character->schema.speed / (float)masterSpeed;
}

// INTELLIGENCE FACTOR
// Brief: Floating point representing current intelligence against the target master intelligence
// CurrentIntelligence / MasterIntelligence
// Can result in 0-1 values for most, exceeding 1 for masters
float RpgSystem::calculateIntelligenceFactor(Character* character)
{
    return (float)character->schema.intelligence / (float)masterIntelligence;
}


int RpgSystem::masterArmorSignificanceHead = 2;
int RpgSystem::masterArmorSignificanceChest = 5;
int RpgSystem::masterArmorSignificanceLegs = 4;
int RpgSystem::masterArmorSignificanceHands = 1;
int RpgSystem::masterArmorSignificanceFeet = 1;


// The total maximum effectiveness of the perfect set of armor
// which mitigates all damage
int RpgSystem::calculateMasterArmorSignificanceTotal()
{
    return masterArmorSignificanceHead + masterArmorSignificanceChest + masterArmorSignificanceLegs + masterArmorSignificanceHands + masterArmorSignificanceFeet;
}


// INITIATIVE
// How fast someone can react and how smart they are to detect the hostility
// INIT = 2 * SpeedFactor * IntelligenceFactor
float RpgSystem::Combat::calculateInitiative(Character* character)
{
    return 2.0f * calculateSpeedFactor(character) * calculateIntelligenceFactor(character);
}


// CHANCE OF HIT
// - Subdivided into three routines: ranged, melee and unarmed
float RpgSystem::Combat::calculateChanceOfHit(Character* attacker, Character* defender)
{
    // If we have a weapon equipped
    Item* weapon = attacker->equipment[Character::EquipmentSlot::WeaponPrimary];
    if(weapon)
    {
        // If it is ranged
        if(weapon->schema.isOfWeaponType(ItemCategory::Weapon::WeaponRanged))
        {
            return calculateRangedChanceOfHit(weapon, attacker, defender);
        }
        
        // If it is melee
        if(weapon->schema.isOfWeaponType(ItemCategory::Weapon::WeaponMelee))
        {
            return calculateMeleeChanceOfHit(weapon, attacker, defender);
        }
    }

    // If we are unarmed
    return calculateUnarmedChanceOfHit(attacker, defender);
}

// RANGED CHANCE OF HIT
// Ranged CoH = (1 - ObstructionPenalty) * RangeFactor * DexFactor
float RpgSystem::Combat::calculateRangedChanceOfHit(Item* weapon, Character* attacker, Character* defender)
{
    return (1 - calculateRangedObstructionPenalty(attacker, defender)) * calculateRangedDistanceFactor(weapon, attacker, defender) * calculateDexterityFactor(attacker);
}

// MELEE CHANCE OF HIT
// See core melee but capped at effective range of weapon
float RpgSystem::Combat::calculateMeleeChanceOfHit(Item* weapon, Character* attacker, Character* defender)
{
    // Ensure we are within effective range
    float ER = weapon->schema.effectiveRangeInTiles;
    float D = bit::VectorMath::distance(attacker->Body::schema.x, attacker->Body::schema.y, defender->Body::schema.x, defender->Body::schema.y) / attacker->level->tileWidth / 1.5; // / 1.5 so we can hit diagonally as well as cardinally
    if(D > ER)
        return 0;

    return calculateCoreMeleeChanceOfHit(attacker, defender);
}

// UNARMGED CHANCE OF HIT
// See core melee but capped at one tile
float RpgSystem::Combat::calculateUnarmedChanceOfHit(Character* attacker, Character* defender)
{
    float D = bit::VectorMath::distance(attacker->Body::schema.x, attacker->Body::schema.y, defender->Body::schema.x, defender->Body::schema.y) / attacker->level->tileWidth / 1.5; // / 1.5 so we can hit diagonally as well as cardinally
    if(D > 1)
        return 0;
    
    return calculateCoreMeleeChanceOfHit(attacker, defender);
}

// CORE MELEE CHANCE OF HTI
// ADB = Attacker Dodge Bypass
// DDR = Defender Dodge Rating
// ABB = Attacker Block Bypass
// DBR = Defender Block Rating
// DCD = Defender Can Dodge
// DBD = Defender Can Block
// AR = Attack Rating
// DR = Defense Rating
// Formula:
// AR = ADB + ABB
// DR = DCD ? DDR : 0 + DCB ? DBR : 0
// COH = AR / (AR + DR)
float RpgSystem::Combat::calculateCoreMeleeChanceOfHit(Character* attacker, Character* defender)
{
    float defenseRating = calculateDodgeRating(defender) + calculateBlockRating(defender);
    float attackRating = calculateDodgeBypass(attacker) + calculateBlockBypass(attacker);
    float coh = attackRating / (attackRating + defenseRating);
    
    return bit::Math::clamp(coh, .05f, .95f);
}


// OBSTRUCTION PENALTY
// Brief: The largest obstruction in the line of sight, always between 0 - 1
// - all bodies contain a 0-1 obstruction percentage
// 	 (Defined by how much of a 1x1x1 object would be blocked by this object)
// - take the max score of all bodies in the LoS
float RpgSystem::Combat::calculateRangedObstructionPenalty(Character* attacker, Character* defender)
{
    float maximum = 0;
    bool first = true;
    attacker->inspectLineOfSightTiles(defender->Body::schema.x, defender->Body::schema.y, [defender, &maximum, &first] (Tile* t) {
        // If we ever hit a full obstruction max it
        if(t->body && t->body != defender && t->body->schema.obstructionRatio == 1)
        {
            maximum = 1;
        }

        // Do not add the obstruction value of any adjacent body so it will be cover
        if(first)
        {
            first = false;
            return;
        }

        // Do not add the obstruction value of our target
        if(t->body && t->body != defender && t->body->schema.obstructionRatio > maximum)
        {
            maximum = t->body->schema.obstructionRatio;
        }
    });
    return maximum;
}

// RANGE FACTOR
// Brief: 100% chance to hit at 0 distance, 50% at effective range of weapon
// (ER / (ER + D))
// ER = Weapon Effect Range Stat (measured in distance of tiles)
// D = Distance to target (in tiles)
// MR = Maximum Range of Weapon
float RpgSystem::Combat::calculateRangedDistanceFactor(Item* weapon, Character* attacker, Character* defender)
{
    float ER = weapon->schema.effectiveRangeInTiles;
    float MR = weapon->schema.maximumRangeInTiles;
    float D = bit::VectorMath::distance(attacker->Body::schema.x, attacker->Body::schema.y, defender->Body::schema.x, defender->Body::schema.y) / attacker->level->tileWidth;
    if(ER == 0 && D == 0)
        return 0;
    if (D > MR)
        return 0;
    return (ER / (ER + D));
}

float RpgSystem::Combat::calculateDodgeBypass(Character* attacker)
{
    return calculateDexterityFactor(attacker);
}

float RpgSystem::Combat::calculateBlockBypass(Character* attacker)
{
    return calculateStrengthFactor(attacker);
}

float RpgSystem::Combat::calculateDodgeRating(Character* defender)
{
    return defender->canDodge() ? calculateDexterityFactor(defender) : 0;
}

float RpgSystem::Combat::calculateBlockRating(Character* defender)
{
    return defender->canBlock() ? calculateStrengthFactor(defender) : 0;
}

float RpgSystem::Combat::calculateAttackDamage(Character* attacker, Character* defender)
{
    // If we have a weapon equipped
    Item* weapon = attacker->equipment[Character::EquipmentSlot::WeaponPrimary];
    if(weapon)
    {
        // If it is ranged
        if(weapon->schema.isOfWeaponType(ItemCategory::Weapon::WeaponRanged))
        {
            return calculateRangedAttackDamage(weapon, attacker, defender);
        }
        
        // If it is melee
        if(weapon->schema.isOfWeaponType(ItemCategory::Weapon::WeaponMelee))
        {
            return calculateMeleeAttackDamage(weapon, attacker, defender);
        }
    }

    // If we are unarmed
    return calculateUnarmedAttackDamage(attacker, defender);
}

// RANGED DAMAGE
// Purely based off of weapon damage
// DMG = rand(MinD, MaxD)
// MinD = weapon minimum damage
// MaxD = weapon maximum damage
float RpgSystem::Combat::calculateRangedAttackDamage(Item* weapon, Character* attacker, Character* defender)
{
    return bit::Math::random(weapon->schema.minimumDamage, weapon->schema.maximumDamage);
}

// MELEE DAMAGE
// Weapon damage time strength factor times 2
// DMG = rand(MinD, MaxD) * SF * 2
// MinD = weapon minimum damage
// MaxD = weapon maximum damage
// SF = attaker strength factor
float RpgSystem::Combat::calculateMeleeAttackDamage(Item* weapon, Character* attacker, Character* defender)
{
    return bit::Math::random(weapon->schema.minimumDamage, weapon->schema.maximumDamage) * calculateStrengthFactor(attacker) * 2.0f;
}

// UNARMED DAMAGE
// Static damage of 5 times strength factor times 2
// DMG = 5 * SF * 2
// SF = attaker strength factor
float RpgSystem::Combat::calculateUnarmedAttackDamage(Character* attacker, Character* defender)
{
    return 5.0f * calculateStrengthFactor(attacker) * 2.0f;
}

// DAMAGE MITIGATION
// DMG = DMG - (DMG * ( ( HAE * HAS + CAE * CAS + LAE * LAS + NAE * NAS + FAE * FAS ) / TA ) )
// DMG = damage
// HAE = Head Armor Effectiveness
// HAS = Head Armor Significance
// CAE = Chest Armor Effectiveness
// CAS = Chest Armor Significance
// LAE = Leg Armor Effectivness
// LAS = Leg Armor Significance
// NAE = Hand Armor Effectiveness
// NAS = Hand Armor Significance
// FAE = Feet Armor Effectiveness
// FAS = Feet Armor Significance
// TA = Total Armor Significance
// Each armor has a damageMitigation float between 0 - 1
// As well, some armors are more useful than others, i.e. chest coverage is better than hands
// Each armor therefore can have an effectiveness (0-1) float and a significance
// Combining all armor effectiveness with their significance and dividing over the total max significance
// results in a percentage of damage it can mitigate
// Subtract the mitigated value from the damage to reduce it
float RpgSystem::Combat::calculateDamageMitigation(Character* attacker, Character* defender, float damage)
{
    // Get armors
    Item* headArmor = defender->equipment[Character::EquipmentSlot::Head];
    Item* chestArmor = defender->equipment[Character::EquipmentSlot::Chest];
    Item* legsArmor = defender->equipment[Character::EquipmentSlot::Legs];
    Item* handsArmor = defender->equipment[Character::EquipmentSlot::Hands];
    Item* feetArmor = defender->equipment[Character::EquipmentSlot::Feet];

    // Get the armor effectiveness against their target master values
    float effectiveness = 0.0f;
    effectiveness += headArmor ? headArmor->schema.armorEffectiveness * masterArmorSignificanceHead : 0;
    effectiveness += chestArmor ? chestArmor->schema.armorEffectiveness * masterArmorSignificanceChest : 0;
    effectiveness += legsArmor ? legsArmor->schema.armorEffectiveness * masterArmorSignificanceLegs : 0;
    effectiveness += handsArmor ? handsArmor->schema.armorEffectiveness * masterArmorSignificanceHands : 0;
    effectiveness += feetArmor ? feetArmor->schema.armorEffectiveness * masterArmorSignificanceFeet : 0;
    
    // Calculate the total effectiveness of armor
    effectiveness = effectiveness / (float)calculateMasterArmorSignificanceTotal();

    // subtract effectiveness from the incoming damage
    return damage - (damage * effectiveness);
}