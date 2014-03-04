#pragma once
#ifndef BIT_MUSICMANAGER_H
#define BIT_MUSICMANAGER_H

#include "SFML/Audio.hpp"
#include "../Game/GameComponent.hpp"
#include "../Math/Math.hpp"
#include <vector>

namespace bit
{
    class Game;

    class MusicManager : public GameComponent
    {
    public:
        MusicManager(Game* game);

        ~MusicManager();

        std::vector<sf::Music*> musicStreams;

        float getMaximum();

        unsigned int loadMusic(std::string fileName);

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