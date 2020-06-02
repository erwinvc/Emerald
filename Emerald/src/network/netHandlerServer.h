#pragma once

class Server;
class NetHandlerServer
{
private:
	Server* m_server;
public:
	NetHandlerServer(Server* server) : m_server(server) {}

	void HandlePacket(const ReceivedPacket& packet, ENetPeer* peer);

	void BroadcastTime(uint64 time);
	void BroadcastEntityStates();
	void BroadcastPlayerJoin(uint32 playerId);
	void BroadcastPlayerLeave(uint32 playerId);
	void BroadcastServerShutdown();
	void OnPlayerState(const PacketPlayerState& packet, ENetPeer* peer);
	void OnHandshake(const PacketHandshake& packet, ENetPeer* peer);
	void OnHandshakeResponse(const PacketHandshakeResponse& packet, ENetPeer* peer);
	void OnBlockUpdate(const PacketBlockUpdate& packet, ENetPeer* peer);
	void OnPlayerDisconnect(ENetPeer* peer);
};