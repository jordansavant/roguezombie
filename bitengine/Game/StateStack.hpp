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

        virtual ~StateStack();

        enum Action
        {
            Push,
            Pop,
            Clear
        };

        virtual void update(sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        template <typename T, class G>
	    void registerState(G* game, unsigned int stateId);

        template <typename T, class G, typename Param1>
	    void registerState(G* game, unsigned int stateId, Param1 arg1);

        template <typename T, class G, typename Param1, typename Param2>
	    void registerState(G* game, unsigned int stateId, Param1 arg1, Param2 arg2);

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

template <typename T, class G>
void bit::StateStack::registerState(G* game, unsigned int stateId)
{
	factories[stateId] = [this, game] () -> T*
	{
		T* t = new T(*this, game);
        t->load();
        return t;
	};
}

template <typename T, class G, typename Param1>
void bit::StateStack::registerState(G* game, unsigned int stateId, Param1 arg1)
{
	factories[stateId] = [this, game, arg1] () -> T*
	{
		T* t = new T(*this, game, arg1);
        t->load();
        return t;
	};
}

template <typename T, class G, typename Param1, typename Param2>
void bit::StateStack::registerState(G* game, unsigned int stateId, Param1 arg1, Param2 arg2)
{
	factories[stateId] = [this, game, arg1, arg2] () -> T*
	{
		T* t = new T(*this, game, arg1, arg2);
        t->load();
        return t;
	};
}

#endif