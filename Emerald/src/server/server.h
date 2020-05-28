#pragma once

class Server : public Singleton<Server> {
private:
	Server() : m_host({ { ENET_HOST_ANY, 25565 }, 4 }) {
		m_salt = Random::Int(0, Math::MAX_INT);
	}
	~Server() {}
	friend Singleton;

	Thread* m_serverThread;
	NetHost m_host;
	ServerWorld m_world;
	std::unordered_map<uint32, int> m_clientsMap;
	vector<PendingClientSession> m_pendingConnections;
	vector<ClientSession> m_clients;
	
	Timer m_timer;

	uint32 m_salt;
	float m_totalFrameTime = 0;
	uint64 m_frameCount = 0;
	float m_lastFrameTime = 0;
	int m_fps = 0;

	void HandlePacket(const ReceivedPacket& packet, ENetPeer* peer);
	void CreatePendingConnection(ENetPeer* peer);
	uint32 CreateClientSession(ENetPeer* peer, uint32 salt);

	//int CreateClientSession(ENetPeer* peer, uint32 salt);
	//void BroadcastEntityStates();
	//void BroadcastPlayerJoin(uint32 playerId);
	//void BroadcastPlayerLeave(uint32 playerId);
	//void BroadcastServerShutdown();
	void OnHandshakeOne(const PacketHandshakeOne& packet, ENetPeer* peer);
	void OnHandshakeResponse(const PacketHandshakeResponse& packet, ENetPeer* peer);

public:

	bool m_running = true;
	void Initialize();
	void Run();
	void Update(const TimeStep& time);
	void Cleanup();
};

inline Server* GetServer() { return Server::GetInstance(); }