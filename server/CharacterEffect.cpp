#include "CharacterEffect.hpp"
#include "Character.hpp"
#include "../bitengine/Network.hpp"
#include "ServerEvent.hpp"
#include "Level.hpp"

CharacterEffect::CharacterEffect()
    : type(Type::None), maxTiles(0), tileInterval(0), tileCounter(0)
{
}

void CharacterEffect::run(Character* character)
{
    tileCounter++;
    if (tileCounter % tileInterval == 0)
    {
        onRun(character);
    }
}

bool CharacterEffect::isComplete()
{
    return tileCounter >= maxTiles;
}

void CharacterEffect::onStart(Character* character)
{
    // Event
    character->level->sendEventToAllPlayers([character, this](bit::ServerPacket &packet) {
        packet << sf::Uint32(ServerEvent::CharacterEffectCreated);
        packet << sf::Uint32(this->type);
        packet << sf::Uint32(character->Body::schema.id);
        packet << character->Body::schema.x;
        packet << character->Body::schema.y;
    });
}

void CharacterEffect::onRun(Character* character)
{
    switch (type)
    {
        case Type::Poison:
        {
            // harm the character
            character->harm(damageAmount);

            break;
        }
    }

    // Event
    character->level->sendEventToAllPlayers([character, this](bit::ServerPacket &packet) {
        packet << sf::Uint32(ServerEvent::CharacterEffectRun);
        packet << sf::Uint32(this->type);
        packet << sf::Uint32(character->Body::schema.id);
        packet << character->Body::schema.x;
        packet << character->Body::schema.y;
    });
}

void CharacterEffect::onEnd(Character* character)
{
    // Event
    character->level->sendEventToAllPlayers([character, this](bit::ServerPacket &packet) {
        packet << sf::Uint32(ServerEvent::CharacterEffectRemoved);
        packet << sf::Uint32(this->type);
        packet << sf::Uint32(character->Body::schema.id);
        packet << character->Body::schema.x;
        packet << character->Body::schema.y;
    });
}

CharacterEffect* CharacterEffect::poison(int _damageAmount, unsigned int _tileInterval, unsigned int _maxTiles)
{
    CharacterEffect* e = new CharacterEffect();
    e->type = Type::Poison;
    e->damageAmount = _damageAmount;
    e->tileInterval = _tileInterval;
    e->maxTiles = _maxTiles;

    return e;
}

std::string CharacterEffect::getTitle(Type type)
{
    switch (type)
    {
        default:
            return "Unknown";
        case Type::Poison:
            return "Poison";
    }
}

std::string CharacterEffect::getAdjective(Type type)
{
    switch (type)
    {
        default:
            return "Unknown";
        case Type::Poison:
            return "Poisoned";
    }
}