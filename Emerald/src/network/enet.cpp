#include "stdafx.h"

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
