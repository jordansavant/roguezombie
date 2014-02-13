#include "SoundManager.h"
#include "SFML/Audio.hpp"
#include "../Game/GameComponent.h"
#include "../Math.h"
#include <vector>

bit::SoundManager::SoundManager()
{
    masterVolume = getMaximum();
    defaultPitch = 1;
}

bit::SoundManager::~SoundManager()
{
    for(int i = 0; i < soundBuffers.size(); i++)
    {
        delete soundBuffers.at(i);
        delete sounds.at(i);
    }
}

float bit::SoundManager::getMaximum()
{
    return 100;
}

int bit::SoundManager::loadSound(std::string fileName)
{
    sf::Sound *sound = new sf::Sound();
    sf::SoundBuffer *soundBuffer = new sf::SoundBuffer();
    if(!soundBuffer->loadFromFile(fileName))
    {
        delete sound;
        delete soundBuffer;
        sound = NULL;
        soundBuffer = NULL;

        return -1;
    }

    sound->setBuffer(*soundBuffer);
    soundBuffers.push_back(soundBuffer);
    sounds.push_back(sound);

    return soundBuffers.size() - 1;
}

void bit::SoundManager::play(int soundId)
{
    play(soundId, masterVolume, defaultPitch);
}

void bit::SoundManager::play(int soundId, float volume, float pitch)
{
    if(soundId >= 0 && soundBuffers.size() > soundId)
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