#include "stdafx.h"

void Network::GetServerStatus(const String& ip, int port, ServerStatus* result) {
	if (result == nullptr) {
		LOG_WARN("[~cNetwork~x] server status result pointer is null");
		return;
	}

	if (result->active) {
		LOG_WARN("[~cNetwork~x] server status request already active");
		return;
	}

	GetThreadPool()->DoJob([result, ip, port] {
		//LOG("[~cNetwork~x] server status request started %s:%d", ip.c_str(), port);
		LOG("[~cNetwork~x] server status request started");
		result->active = true;
		result->status = ServerStatusResult::Pending;

		NetHost host;
		Connection serverConnection;
		if (ConnectClientToServer(host.m_handle, serverConnection, ip, port)) {
			PacketHandshake packet;
			packet.salt = 0;
			packet.handshakeType = HandshakeType::Status;
			serverConnection.SendPacket(&packet);


			bool received = false;
			int timeOutTimer = 0;
			while (!received && timeOutTimer < 50) {
				NetEvent evnt;
				if (host.GetEvent(evnt)) {
					if (evnt.type == NetEventType::Data) {
						ReceivedPacket packet(evnt.packet);
						if (packet.type == PacketType::ServerStatus) {
							LOG("[~cNetwork~x] server status request succesful");
							result->status = ServerStatusResult::Succesful;
							result->active = false;
							result->maxPlayers = packet.As<PacketServerStatus>().maxPlayers;
							result->playerCount = packet.As<PacketServerStatus>().playerCount;
							result->name = packet.As<PacketServerStatus>().serverTitle;
							result->motd = packet.As<PacketServerStatus>().serverMotd;
							received = true;
						}
						evnt.Destroy();
					}
				}
				timeOutTimer++;
				Sleep(100);
			}
			if (!received) {
				LOG("[~cNetwork~x] server status request timed out");
				result->status = ServerStatusResult::TimedOut;
				result->active = false;
			}
		} else {
			LOG("[~cNetwork~x] server status request failed to connect to server");
			result->status = ServerStatusResult::Failed;
			result->active = false;
		}
	});
}

void NetEvent::Destroy() {
	enet_packet_destroy(data.packet);
}

NetHost::NetHost() {
	m_handle = enet_host_create(nullptr, 1, 2, 0, 0);
}

NetHost::NetHost(const ENetAddress& address, int maximumConnections) {
	m_handle = enet_host_create(&address, maximumConnections, 2, 0, 0);
	if (!m_handle) {
		LOG_ERROR("[~cNetwork~x] failed to create server");
	} else LOG("[~cNetwork~x] created server");
}

NetHost::~NetHost() {
	if (m_handle) {
		enet_host_destroy(m_handle);
	}
}

bool NetHost::GetEvent(NetEvent& event) {
	if (enet_host_service(m_handle, &event.data, 0)) {
		event.type = static_cast<NetEventType>(event.data.type);
		event.packet = event.data.packet;
		event.peer = event.data.peer;
		return true;
	}
	return false;
}

void Connection::Send(const void* data, uint size, int channel, uint32 flags) {
	auto enetPacket = CreatePacket(data, size, flags);
	enet_peer_send(m_peer, channel, enetPacket);
}
void Connection::Send(const PacketWriter& writer, int channel, uint32 flags)
{
	auto enetPacket = CreatePacket(writer.GetData(), writer.GetSize(), flags);
	enet_peer_send(m_peer, channel, enetPacket);
}

void Connection::SendPacketWriter(const PacketWriter& writer, int channel, uint32 flags) {
	auto enetPacket = CreatePacket(writer.GetData(), writer.GetSize(), flags);
	enet_peer_send(m_peer, channel, enetPacket);
}
