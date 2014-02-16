#pragma once
#ifndef BIT_LEVELCOMPONENT_H
#define BIT_LEVELCOMPONENT_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class Camera;

    class LevelComponent
    {
    public:
        LevelComponent();

        virtual ~LevelComponent();

        enum LevelComponentState
        {
            Loading,
            WaitingForOtherComponentsToLoad,
            Running,
            Unloading,
            Unloaded,
            UnloadingForReload,
            UnloadedWaitingForOtherComponentsToUnloadForReload
        };

        bool isPausable;
        LevelComponentState state;
        bool isLoaded;
        float percentageLoaded;

        virtual bool load(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera);

        virtual bool unload(sf::RenderWindow &window, sf::Time &gameTime);

        void updateState(sf::RenderWindow &window, sf::Time &gameTime, bool allComponentsLoaded, bool allComponentsUnloaded);
    };
}

#endif