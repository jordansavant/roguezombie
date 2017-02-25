#include "MusicManager.hpp"
#include "SFML/Audio.hpp"
#include "../Game/Game.hpp"
#include "../Game/GameComponent.hpp"
#include "../Math/Math.hpp"
#include "../Structures/Exception.hpp"
#include <vector>

bit::MusicManager::MusicManager(Game* _game)
    : bit::GameComponent(_game)
{
    masterVolume = getMaximum();
    defaultPitch = 1;
}

bit::MusicManager::~MusicManager()
{
    for(unsigned int i = 0; i < musicStreams.size(); i++)
    {
        delete musicStreams[i];
    }
}

float bit::MusicManager::getMaximum()
{
    return 100;
}

unsigned int bit::MusicManager::loadMusic(std::string fileName)
{
    sf::Music *music = new sf::Music();
    if(!music->openFromFile(fileName))
    {
        delete music;
        music = NULL;

        throw bit::Exception(fileName + " music not found.");
    }

    musicStreams.push_back(music);

    return musicStreams.size() - 1;
}

void bit::MusicManager::play(unsigned int musicId)
{
    if(musicStreams.size() > musicId)
    {
        sf::Music* music = musicStreams.at(musicId);

        music->play();
    }
}

void bit::MusicManager::loop(unsigned int musicId, bool loop)
{
    if(musicStreams.size() > musicId)
    {
        sf::Music* music = musicStreams.at(musicId);

        music->setLoop(loop);
    }
}

float bit::MusicManager::getMasterVolume()
{
    return masterVolume;
}

void bit::MusicManager::setMasterVolume(float volume)
{
    masterVolume = bit::Math::clamp(volume, 0, getMaximum());

    for(unsigned int i = 0; i < musicStreams.size(); i++)
    {
        sf::Music* music = musicStreams.at(i);
        music->setVolume(masterVolume);
    }
}

void bit::MusicManager::changeVolume(float amount, bool wrapAround)
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

    for(unsigned int i = 0; i < musicStreams.size(); i++)
    {
        sf::Music* music = musicStreams.at(i);
        music->setVolume(masterVolume);
    }
}