#include "RemoteClient.hpp"
#include "SFML/Network.hpp"

bit::RemoteClient::RemoteClient()
    : id(0), isConnected(false), isConfirmed(false), hasTimedOut(false), hasDisconnected(false), lastAcknowledgedSnapshotId(0)
{
    socket.setBlocking(false);
}