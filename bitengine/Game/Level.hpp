#pragma once
#ifndef BIT_LEVEL_H
#define BIT_LEVEL_H

#include "../Graphics/Camera.hpp"
#include "LevelComponent.hpp"
#include <vector>

namespace bit
{
    class Game;

    class Level
    {
    public:
        Level(sf::Vector2i targetLevelResolution);

        virtual ~Level();

        enum LevelStateStep
        {
            Loading,
            Running,
            Unloading,
            Unloaded,
            UnloadingForReload
        };

        sf::Vector2i targetLevelResolution;
        std::vector<LevelComponent*> levelComponents;
        std::vector<Camera*> cameras;
        LevelStateStep currentStep;
        bool allComponentsLoaded;
        bool allComponentsUnloaded;

        virtual void handleWindowChange(sf::RenderWindow &window);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void drawComponents(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera);

        virtual float getPercentageLoaded();

        void unloadComponents();

        virtual void reloadComponents();

        Camera* createCamera(sf::RenderWindow &window, float x, float y, float width, float height);

        // Level Hooks
        virtual void initialize(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void loadingUpdate(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void runningUpdate(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void unloadingUpdate(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void loadingDraw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void runningDraw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void unloadingDraw(sf::RenderWindow &window, sf::Time &gameTime);

    private:
        bool isInitialized;
    };
}

#endif