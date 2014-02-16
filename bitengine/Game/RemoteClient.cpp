#include "RemoteClient.hpp"
#include "SFML/Network.hpp"

bit::RemoteClient::RemoteClient()
    : isReady(false), hasTimedOut(false)
{
    socket.setBlocking(false);
}
