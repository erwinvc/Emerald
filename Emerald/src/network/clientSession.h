#pragma once

struct PendingClientSession {
	Connection connection;
	uint32 salt = 0;
	char name[16];
};

class ClientSession {
private:
	Connection m_clientConnection;
	uint32 m_salt = 0;
	uint32 m_playerId = 0;

	bool m_isActive = false;

	int dist = 4;
public:
	char m_name[16];

	void Initialize(ENetPeer* peer, uint32 salt, String_t name, uint32 playerId);
	void Disconnect();

	template<typename T>
	void SendPacket(const T* packet, uint32 channel = 0, uint32 flags = 0) {
		m_clientConnection.Send(&packet, sizeof(T), channel, flags);
	}

	void Send(const PacketWriter& writer, uint32 channel = 0, uint32 flags = 0) {
		m_clientConnection.Send(writer, channel, flags);
	}

	void tick();

	bool Verify(uint32 salt) const { return salt == m_clientConnection.m_salt; }
	bool IsActive() const { return m_isActive; }
	uint32 GetPlayerId() const { return m_playerId; }
};