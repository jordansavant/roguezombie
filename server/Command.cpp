#include "Command.hpp"
#include "SFML/Network.hpp"

Command::Command()
    : type(Type::None), pack([] (sf::Packet &packet) {})
{
}

Command::Command(Type _type)
	: type(_type), pack([](sf::Packet &packet) {})
{
}