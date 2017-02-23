#pragma once
#ifndef BIT_EVENT_H
#define BIT_EVENT_H

#include <vector>

namespace bit
{
    template<typename T>
    class Event
    {
    private:
        std::vector<T> listeners;
    
    public:
        template<typename A>
        void trigger(A arg1)
        {
            for(unsigned int i=0; i < listeners.size(); i++)
                listeners[i](arg1);
        }
        template<typename A, typename B>
        void trigger(A arg1, B arg2)
        {
            for(unsigned int i=0; i < listeners.size(); i++)
                listeners[i](arg1, arg2);
        }
        template<typename A, typename B, typename C>
        void trigger(A arg1, B arg2, C arg3)
        {
            for(unsigned int i=0; i < listeners.size(); i++)
                listeners[i](arg1, arg2, arg3);
        }

        // Registers the listener and returns its ID if it needs to remove it
        unsigned int operator += (T listener)
        {
            listeners.push_back(listener);
            return listeners.size() - 1;
        }

        // Removes a listener by its id
        void operator -= (unsigned int index)
        {
            listeners.erase(listeners.begin() + index);
        }
    };
}

#endif