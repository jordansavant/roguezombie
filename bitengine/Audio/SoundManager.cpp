#include "SoundManager.hpp"
#include "SFML/Audio.hpp"
#include "../Game/Game.hpp"
#include "../Game/GameComponent.hpp"
#include "../Math/Math.hpp"
#include "../Structures/Exception.hpp"
#include <vector>

bit::SoundManager::SoundManager(bit::Game* _game)
    : bit::GameComponent(_game)
{
    masterVolume = getMaximum();
    defaultPitch = 1;
}

bit::SoundManager::~SoundManager()
{
    for(unsigned int i = 0; i < soundBuffers.size(); i++)
    {
        delete soundBuffers.at(i);
        delete sounds.at(i);
    }
}

float bit::SoundManager::getMaximum()
{
    return 100;
}

unsigned int bit::SoundManager::loadSound(std::string fileName)
{
    sf::Sound *sound = new sf::Sound();
    sf::SoundBuffer *soundBuffer = new sf::SoundBuffer();
    if(!soundBuffer->loadFromFile(fileName))
    {
        delete sound;
        delete soundBuffer;
        sound = NULL;
        soundBuffer = NULL;

        throw bit::Exception(fileName + " sound not found.");
    }

    sound->setBuffer(*soundBuffer);
    soundBuffers.push_back(soundBuffer);
    sounds.push_back(sound);

    return soundBuffers.size() - 1;
}

void bit::SoundManager::play(unsigned int soundId)
{
    play(soundId, masterVolume, defaultPitch);
}

void bit::SoundManager::play(unsigned int soundId, float volume, float pitch)
{
    if(soundBuffers.size() > soundId)
    {
        sf::Sound* sound = sounds.at(soundId);

        sound->setVolume(Math::clamp(volume, 0, masterVolume));
        sound->setPitch(pitch);
        sound->play();
    }
}

float bit::SoundManager::getMasterVolume()
{
    return masterVolume;
}

void bit::SoundManager::setMasterVolume(float volume)
{
    masterVolume = Math::clamp(volume, 0, getMaximum());
}

void bit::SoundManager::setByRatio(float ratio)
{
    ratio = bit::Math::clamp(ratio, 0, 1);
    setMasterVolume(ratio * getMaximum());
}

void bit::SoundManager::changeVolume(float amount, bool wrapAround)
{
    if (masterVolume + amount < 0)
    {
        if (wrapAround)
        {
            masterVolume = getMaximum();
        }
        else
        {
            masterVolume = 0;
        }
    }
    else if (masterVolume + amount > getMaximum())
    {
        if (wrapAround)
        {
            masterVolume = 0;
        }
        else
        {
            masterVolume = getMaximum();
        }
    }
    else
    {
        masterVolume += amount;
    }
}