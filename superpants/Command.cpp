#include "Command.hpp"
#include "SFML/Network.hpp"

Command::Command()
    : type(Type::None), pack([] (sf::Packet &packet) {})
{
}