#pragma once
#ifndef BIT_STATE_H
#define BIT_STATE_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class StateStack;
    class Camera;
    class Game;

    class State
    {
    public:

        State(StateStack &stack, Game* game);

        virtual bool handleInput(sf::RenderWindow &window, sf::Time &gameTime);

        virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera);

        Camera* createCamera(sf::RenderWindow &window, float x, float y, float width, float height);

        void requestStackPush(unsigned int stateId);

        void requestStackPop();

        void requestStateClear();

        virtual void handleWindowChange(sf::RenderWindow &window);
        
    private:

        StateStack* stateStack;

    protected:

        Game* game;
        std::vector<Camera*> cameras;

    };
}

#endif