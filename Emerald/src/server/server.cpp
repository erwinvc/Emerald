#include "stdafx.h"

void Server::Initialize() {
	m_netHandler = NEW(NetHandlerServer(this));
	m_world = NEW(ServerWorld());
	Run();
}

void Server::Run() {
	while (m_running) {
		m_updateTimer.Update();
		for (int i = 0; i < Math::Min(10, m_updateTimer.m_elapsedUpdates); i++) {
			Update();
		}
	}
	Shutdown();
}
void Server::Update() {
	m_world->Update();
	NetEvent evnt;
	while (m_host.GetEvent(evnt)) {
		switch (evnt.type) {
			case NetEventType::Connection:
				CreatePendingConnection(evnt.peer);
				break;

			case NetEventType::Disconnect:
			case NetEventType::Timeout:
				m_netHandler->OnPlayerDisconnect(evnt.peer);
				break;

			case NetEventType::Data: {
				ReceivedPacket packet(evnt.packet);
				m_netHandler->HandlePacket(packet, evnt.peer);
				evnt.Destroy();
			} break;
		}
	}

	for (auto& client : m_clients) {
		if (client.IsActive()) {
			client.tick();
		}
	}

	if (++m_timeCounter > 60) {
		m_timeCounter = 0;
		m_netHandler->BroadcastTime(m_world->GetTime());
	}

	m_netHandler->BroadcastEntityStates();
}

void Server::Shutdown() {
	LOG("Shutdown!");
	m_netHandler->BroadcastServerShutdown();
	enet_host_flush(m_host.m_handle);
	for (auto& session : m_clients) {
		session.Disconnect();
		LOG("Disconnect %s!", session.m_name);
	}
}

void Server::CreatePendingConnection(ENetPeer* peer) {
	LOG("[~cNetwork~x] new client connected %s", peer->address.host.u.Byte);
	PendingClientSession session;
	session.connection.m_peer = peer;
	m_pendingConnections.push_back(session);
}

uint32 Server::CreateClientSession(ENetPeer* peer, String_t name, uint32 salt) {
	for (unsigned i = 0; i < m_clients.size(); i++) {
		if (!m_clients[i].IsActive()) {
			uint32 playerId = m_world->AddEntity();
			strcpy(m_world->GetEntity(playerId).name, name);
			m_clients[i].Initialize(peer, salt, name, playerId);
			m_clientsMap[peer->incomingPeerID] = i;

			m_netHandler->BroadcastPlayerJoin(playerId);
			return i;
		}
	}
	return -1;
}

void Server::Cleanup() {
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(m_world);
	GetMemory()->CheckAllocations();
}