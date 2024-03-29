#pragma once
#ifndef BIT_STATE_H
#define BIT_STATE_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class StateStack;
    class Camera;
    class Game;
    class VideoGame;

    class State
    {
    public:

        State(StateStack &stack, Game* game);

        virtual ~State();

        Game* game;

        virtual bool handleInput(sf::Time &gameTime);

        virtual void load();

        virtual bool update(sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera);

        Camera* createCamera(bit::VideoGame* videoGame, float x, float y, float width, float height);

        void requestStackPush(unsigned int stateId);

        void requestStackPop();

        void requestStateClear();

        void requestStateClearTo(unsigned int stateId);

        virtual void handleWindowChange(sf::RenderWindow &window);

    private:

        StateStack* stateStack;

    protected:

        std::vector<Camera*> cameras;
        bool isPaused;
    };
}

#endif