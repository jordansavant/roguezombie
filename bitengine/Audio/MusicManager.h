#pragma once
#ifndef BIT_MUSICMANAGER_H
#define BIT_MUSICMANAGER_H

#include "SFML/Audio.hpp"
#include "../Game/GameComponent.h"
#include "../MathX.h"
#include <vector>

namespace bit
{
    class MusicManager : public GameComponent
    {
    public:
        MusicManager();

        ~MusicManager();

        std::vector<sf::Music*> musicStreams;

        float getMaximum();

        int loadMusic(std::string fileName);

        void play(unsigned int musicId);

        float getMasterVolume();

        void setMasterVolume(float volume);

        void changeVolume(float amount, bool wrapAround = false);

    private:
        float masterVolume;
        float defaultPitch;
    };
}

#endif
