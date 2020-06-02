#pragma once

class Client;
class NetHandlerClient
{
private:
	Client* m_client;
public:
	NetHandlerClient(Client* client) : m_client(client) {}
	
	void OnConnectionResult(const PacketConnectionResult& packet);
	void OnKick(const PacketKick& packet);
	void OnGameData(const void* data);
	void OnAddEntity(const void* data);
	void OnRemoveEntity(const PacketRemoveEntity& packet);
	void OnUpdateEntities(const void* data);
	void OnBlockUpdate(const PacketBlockUpdate& packet);
	void OnTime(const PacketTime& packet);
	void HandlePacket(const ReceivedPacket& packet);
	void OnHandshakeChallenge(const PacketHandshakeChallenge& packet);
};