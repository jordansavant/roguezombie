#pragma once
#ifndef BIT_NETWORKHELPER_H
#define BIT_NETWORKHELPER_H

#include "SFML/Network.hpp"

namespace bit
{
    class NetworkHelper
    {
    public:

        template <typename F, typename T>
        static void unpackEnum(sf::Packet &packet, T &value)
        {
            F x;
            packet >> x;
            value = static_cast<T>(x);
        }
    };
}

#endif