#pragma once

class Server : public Singleton<Server> {
private:
	Server() : m_updateTimer(UpdateTimer(60.0f)), m_host({ { ENET_HOST_ANY, 25565 }, 16 }) {
		m_salt = Random::Int(0, Math::MAX_INT);
		m_clients.resize(16);
	}
	~Server() {}
	friend Singleton;

	Thread* m_serverThread;
	NetHost m_host;
	ServerWorld* m_world;
	NetHandlerServer* m_netHandler;

	UpdateTimer m_updateTimer;

	std::unordered_map<uint32, int> m_clientsMap;
	vector<PendingClientSession> m_pendingConnections;
	vector<ClientSession> m_clients;

	uint32 m_salt;
	uint32 m_timeCounter = 0;
	
	void Shutdown();
	void CreatePendingConnection(ENetPeer* peer);
	uint32 CreateClientSession(ENetPeer* peer, String_t name, uint32 salt);

public:
	bool m_running = true;
	void Initialize();
	void Run();
	void Update();
	void Cleanup();

	template<typename T>
	void BroadcastPacket(const T* packet, uint32 channel = 0, uint32 flags = 0) {
		for (auto& client : m_clients) {
			if (client.IsActive())
				client.SendPacket(packet, channel, flags);
		}
	}
	void BroadcastPacket(const PacketWriter& writer, uint32 channel = 0, uint32 flags = 0) {
		for (auto& client : m_clients) {
			if (client.IsActive())
				client.Send(writer, channel, flags);
		}
	}

	ServerWorld* GetWorld() { return m_world; }

	inline uint32 GetPlayerCount() { return m_clientsMap.size(); }
	
	friend NetHandlerServer;
};

inline Server* GetServer() { return Server::GetInstance(); }