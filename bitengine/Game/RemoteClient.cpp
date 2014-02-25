#include "RemoteClient.hpp"
#include "SFML/Network.hpp"

bit::RemoteClient::RemoteClient()
    : isReady(false), hasTimedOut(false), lastAcknowledgedSnapshotId(0)
{
    socket.setBlocking(false);
}
