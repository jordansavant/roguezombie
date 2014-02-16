#pragma once
#ifndef BIT_STATESTACK_H
#define BIT_STATESTACK_H

#include "SFML/Graphics.hpp"
#include "GameComponent.hpp"
#include <functional>
#include <map>

namespace bit
{
    class State;
    class Game;

    class StateStack : public GameComponent
    {
    public:

        StateStack(Game* game);

        enum Action
        {
            Push,
            Pop,
            Clear
        };

        void update(sf::RenderWindow &window, sf::Time &gameTime);

        void draw(sf::RenderWindow &window, sf::Time &gameTime);

        template <typename T>
	    void registerState(unsigned int stateId);

        template <typename T, typename Param1>
	    void registerState(unsigned int stateId, Param1 arg1);

        void pushState(unsigned int StateId);

        void popState();

        void clearStates();

        bool empty();

        virtual void cascadeWindowEvent(sf::RenderWindow &window);

    private:

        struct PendingChange
        {
            explicit PendingChange(Action action, unsigned int stateId);
	        Action action;
	        unsigned int stateId;
        };

        std::map<unsigned int, std::function<State*(void)>> factories;
        std::vector<State*> stack;
        std::vector<PendingChange> pendingChangeList;

        State* createState(unsigned int stateId);

        void applyPendingChanges();
    };
}

template <typename T>
void bit::StateStack::registerState(unsigned int stateId)
{
	factories[stateId] = [this] () -> T*
	{
		T* t = new T(*this, game);
        t->load();
        return t;
	};
}

template <typename T, typename Param1>
void bit::StateStack::registerState(unsigned int stateId, Param1 arg1)
{
	factories[stateId] = [this, arg1] () -> T*
	{
		T* t = new T(*this, game, arg1);
        t->load();
        return t;
	};
}

#endif