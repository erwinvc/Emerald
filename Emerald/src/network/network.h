#pragma once

#include "vendor/enet/enet.h"

enum class ServerStatusResult {
	Nothing,
	Succesful,
	Pending,
	Failed,
	TimedOut
};
struct ServerStatus {
	ServerStatusResult status = ServerStatusResult::Nothing;
	uint32 maxPlayers;
	uint32 playerCount;
	String name;
	String motd;
	bool active = false;

	String ToString() {
		switch (status) {
			case ServerStatusResult::Succesful: return "Succesful";
			case ServerStatusResult::Pending: return "Pending";
			case ServerStatusResult::Failed: return "Failed";
			case ServerStatusResult::TimedOut: return "Timed out";
			default:
			case ServerStatusResult::Nothing: return "Null";
		}
	}
};

class Network {
public:
	static void Initialize() {
		enet_initialize();
	}
	static void Cleanup() {
		enet_deinitialize();
	}

	static void GetServerStatus(const String& ip, int port, ServerStatus* status);
};

static ENetPacket* CreatePacket(const void* data, uint size, uint32 flags) {
	return enet_packet_create(data, size, flags);
}

enum class NetEventType {
	None = ENET_EVENT_TYPE_NONE,
	Data = ENET_EVENT_TYPE_RECEIVE,
	Connection = ENET_EVENT_TYPE_CONNECT,
	Disconnect = ENET_EVENT_TYPE_DISCONNECT,
	Timeout = ENET_EVENT_TYPE_DISCONNECT_TIMEOUT,
};

class NetEvent {
public:
	ENetEvent data;
	NetEventType type;

	ENetPacket* packet = nullptr;
	ENetPeer* peer = nullptr;

	void Destroy();
};

class NetHost {
public:
	ENetHost* m_handle = nullptr;

	NetHost();
	NetHost(const ENetAddress& address, int maximumConnections);
	~NetHost();

	bool GetEvent(NetEvent& event);
};

class PacketWriter;
class Connection {
public:
	ENetPeer* m_peer = nullptr;
	uint32 m_salt = 0;

	void Send(const void* data, uint size, int channel = 0, uint32 flags = 0);
	void Send(const PacketWriter& writer, int channel = 0, uint32 flags = 0);

	template<typename T>
	void SendPacket(const T* data, int channel = 0, uint32 flags = ENET_PACKET_FLAG_RELIABLE) {
		auto enetPacket = CreatePacket((const void*)data, sizeof(T), flags);
		enet_peer_send(m_peer, channel, enetPacket);
	}

	void SendPacketWriter(const PacketWriter& writer, int channel = 0, uint32 flags = ENET_PACKET_FLAG_RELIABLE);

	void Disconnect() {
		enet_peer_disconnect(m_peer, 0);
	}

};

static bool ConnectClientToServer(ENetHost* host, Connection& serverConnection, const String& ipAddress, int port) {
	// Create address for the client to connect to
	ENetAddress address{};
	address.port = port;
	if (enet_address_set_host(&address, ipAddress.c_str()) != 0) {
		//LOG_ERROR("Failed to create address");
		return false;
	}

	serverConnection.m_peer = enet_host_connect(host, &address, 2, 0);
	if (!serverConnection.m_peer) {
		//LOG_WARN("Failed to connect to the server");
		return false;
	}

	// Wait for a connection establishment
	bool connected = [&host] {
		ENetEvent event;
		while (enet_host_service(host, &event, 2000) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
			} else if (event.type == ENET_EVENT_TYPE_CONNECT) {
				return true;
			}
		}
		return false;
	}();
	return connected;
}

static bool DisconnectClient(ENetHost* host, Connection& serverConnection) {
	enet_peer_disconnect(serverConnection.m_peer, 0);
	ENetEvent event;
	while (enet_host_service(host, &event, 2000) > 0) {
		if (event.type == ENET_EVENT_TYPE_RECEIVE) {
			enet_packet_destroy(event.packet);
		} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
			enet_host_flush(host);
			return true;
		}
	}
	enet_peer_reset(serverConnection.m_peer);
	return false;
}

static void BroadcastToPeers(ENetHost* host, const void* data, uint32 size, int channel, uint32 flags) {
	auto enetpacket = CreatePacket(data, size, flags);
	enet_host_broadcast(host, channel, enetpacket);
}