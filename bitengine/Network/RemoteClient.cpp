#include "RemoteClient.hpp"
#include "SFML/Network.hpp"

bit::RemoteClient::RemoteClient()
    : id(0), isNetworkConnected(false), isConfirmed(false), hasTimedOut(false), hasDisconnected(false), hasBeenKicked(false), lastAcknowledgedSnapshotId(0)
{
    socket.setBlocking(false);
}

bool bit::RemoteClient::badConnection()
{
    return hasTimedOut || hasDisconnected || hasBeenKicked;
}

bool bit::RemoteClient::canReceiveGamePackets()
{
    return !badConnection() && isNetworkConnected && isConfirmed;
}