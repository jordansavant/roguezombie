#include "RemoteClient.hpp"
#include "SFML/Network.hpp"

bit::RemoteClient::RemoteClient()
    : isConnected(false), isConfirmed(false), hasTimedOut(false), lastAcknowledgedSnapshotId(0)
{
    socket.setBlocking(false);
}
