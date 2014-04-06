#include "StateStack.hpp"
#include "State.hpp"
#include "SFML/Graphics.hpp"
#include "../Game/Game.hpp"
#include "../Game/GameComponent.hpp"
#include <functional>

bit::StateStack::StateStack(bit::Game*_game)
    : bit::GameComponent(_game)
{
}

bit::StateStack::~StateStack()
{
    for(unsigned int i=0; i < stack.size(); i++)
    {
        delete stack[i];
    }
}

void bit::StateStack::update(sf::RenderWindow &window, sf::Time &gameTime)
{
	applyPendingChanges();

    for(unsigned int i=0; i < stack.size(); i++)
	{
        // If a state steals input do not run the others
		if(!stack[i]->handleInput(window, gameTime))
		{
			break;
		}
	}

    for(unsigned int i=0; i < stack.size(); i++)
	{
        // If a state cancels updating do not update others
		if(!stack[i]->update(window, gameTime))
		{
			return;
		}
	}
}

void bit::StateStack::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i=0; i < stack.size(); i++)
	{
		stack[i]->draw(window, gameTime);
	}
}

void bit::StateStack::pushState(unsigned int stateID)
{
    pendingChangeList.push_back(PendingChange(bit::StateStack::Action::Push, stateID));
}

void bit::StateStack::popState()
{
    pendingChangeList.push_back(PendingChange(bit::StateStack::Action::Pop, 0));
}

void bit::StateStack::clearStates()
{
	pendingChangeList.push_back(PendingChange(bit::StateStack::Clear, 0));
}

bit::State* bit::StateStack::createState(unsigned int stateId)
{
    return factories[stateId]();
}

void bit::StateStack::applyPendingChanges()
{
    for(unsigned int i=0; i < pendingChangeList.size(); i++)
	{
        switch(pendingChangeList[i].action)
		{
            case bit::StateStack::Action::Push:

                stack.push_back(createState(pendingChangeList[i].stateId));

                break;

            case bit::StateStack::Action::Pop:

                delete stack.back();

                stack.pop_back();

                break;

            case bit::StateStack::Action::Clear:

                for(unsigned int i=0; i < stack.size(); i++)
                {
                    delete stack[i];
                }

                stack.clear();

                break;
        }
	}

    pendingChangeList.clear();
}

bool bit::StateStack::empty()
{
    return stack.size() == 0;
}

void bit::StateStack::cascadeWindowEvent(sf::RenderWindow &window)
{
    for(unsigned int i=0; i < stack.size(); i++)
    {
        stack[i]->handleWindowChange(window);
    }
}

bit::StateStack::PendingChange::PendingChange(bit::StateStack::Action action, unsigned int stateId)
    : action(action), stateId(stateId)
{
}