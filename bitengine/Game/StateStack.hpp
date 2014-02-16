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

    class StateStack : public GameComponent
    {
    public:

        StateStack();

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

        void pushState(unsigned int StateId);

        void popState();

        void clearStates();

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
	factories[stateId] = [this] ()
	{
		return new T(*this);
	};
}

#endif