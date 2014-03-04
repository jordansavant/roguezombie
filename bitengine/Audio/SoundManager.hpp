#pragma once
#ifndef BIT_SOUNDMANAGER_H
#define BIT_SOUNDMANAGER_H

#include "../Game/GameComponent.hpp"
#include <string>

namespace sf
{
    class SoundBuffer;
    class Sound;
}

namespace bit
{
    class Game;

    class SoundManager : public GameComponent
    {
    public:

        SoundManager(Game* game);

        ~SoundManager();

        std::vector<sf::SoundBuffer*> soundBuffers;
        std::vector<sf::Sound*> sounds;

        float getMaximum();

        unsigned int loadSound(std::string fileName);

        void play(unsigned int soundId);

        void play(unsigned int soundId, float volume, float pitch = 1);

        float getMasterVolume();

        void setMasterVolume(float volume);

        void changeVolume(float amount, bool wrapAround = false);

    private:

        float masterVolume;
        float defaultPitch;
    };
}

#endif