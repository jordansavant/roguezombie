#pragma once
#ifndef BIT_SOUNDMANAGER_H
#define BIT_SOUNDMANAGER_H

#include "../Game/GameComponent.h"
#include <string>

namespace sf
{
    class SoundBuffer;
    class Sound;
}

namespace bit
{
    class SoundManager : public GameComponent
    {
    public:

        SoundManager();

        ~SoundManager();

        std::vector<sf::SoundBuffer*> soundBuffers;
        std::vector<sf::Sound*> sounds;
        
        float getMaximum();

        int loadSound(std::string fileName);

        void play(int soundId);

        void play(int soundId, float volume, float pitch = 1);

        float getMasterVolume();

        void setMasterVolume(float volume);

        void changeVolume(float amount, bool wrapAround = false);

    private:

        float masterVolume;
        float defaultPitch;
    };
}

#endif