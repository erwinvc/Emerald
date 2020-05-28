#include "stdafx.h"

void Server::Initialize() {
	Run();
}

void Server::Run() {
	m_timer = Timer();
	float timer = m_timer.Get();
	float updateTimer = m_timer.Get();
	float updateTick = 1000.0f / 60.0f;
	double delta = 0;
	int frames = 0, updates = 0;
	while (m_running) {
		float time = m_timer.Get();
		if (time - updateTimer > updateTick) {
			m_totalFrameTime += time - m_lastFrameTime;
			Update(TimeStep(time - m_lastFrameTime, m_totalFrameTime, m_frameCount));

			m_lastFrameTime = time;
			updates++;
			updateTimer += updateTick;
			m_frameCount++;
		}
		delta += (time - updateTimer) / updateTick;
		frames++;
		if (glfwGetTime() - timer > 1.0) {
			m_fps = frames;
			timer++;
			updates = frames = 0;
		}
	}
}

void Server::Update(const TimeStep& time) {
	NetEvent evnt;
	while (m_host.GetEvent(evnt)) {
		switch (evnt.type) {
			case NetEventType::Connection:
				CreatePendingConnection(evnt.peer);
				break;

			case NetEventType::Disconnect:
			case NetEventType::Timeout:
				//handleDisconnection(evnt.peer);
				break;

			case NetEventType::Data: {
				ReceivedPacket packet(evnt.packet);
				HandlePacket(packet, evnt.peer);
				evnt.Destroy();
			} break;
		}
	}

	//for (auto& client : m_clients) {
	//	if (client.isActive()) {
	//		client.tick();
	//	}
	//}
}

void Server::HandlePacket(const ReceivedPacket& packet, ENetPeer* peer) {
	switch (packet.type) {
		case PacketType::HandshakeOne: OnHandshakeOne(packet.As<PacketHandshakeOne>(), peer); break;
		case PacketType::HandshakeResponse: OnHandshakeResponse(packet.As<PacketHandshakeResponse>(), peer); break;
	}
}

void Server::CreatePendingConnection(ENetPeer* peer) {
	LOG("[~cNetwork~x] new client connected %s", peer->address.host.u);
	PendingClientSession session;
	session.connection.m_peer = peer;
	m_pendingConnections.push_back(session);
}

uint32 Server::CreateClientSession(ENetPeer* peer, uint32 salt) {
	for (unsigned i = 0; i < m_clients.size(); i++) {
		if (!m_clients[i].IsActive()) {
			uint32 playerId = m_world.AddEntity();
			m_clients[i].Initialize(peer, salt, playerId);
			m_clientsMap[peer->incomingPeerID] = i;
			//broadcastPlayerJoin(playerId);
			return i;
		}
	}
	return -1;
}

void Server::OnHandshakeOne(const PacketHandshakeOne& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received handshakeone from client");

	for (auto& pending : m_pendingConnections) {
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			pending.salt = packet.salt;
			PacketHandshakeChallenge newPacket;
			newPacket.salt = m_salt;
			pending.connection.SendPacket(&newPacket, 0, ENET_PACKET_FLAG_RELIABLE);
			break;
		}
	}
}

void Server::OnHandshakeResponse(const PacketHandshakeResponse& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received handshake response from client");
	for(auto it = m_pendingConnections.begin(); it != m_pendingConnections.end(); it++){
		auto pending = *it;
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			uint32 salt = pending.salt ^ m_salt;
			if (salt == packet.salt) {
				pending.salt = salt;
				LOG("[~cNetwork~x] client salt accepted!");
				//int slot = createClientSession(peer, salt);
				//if (slot != -1) {
				//	pending.sendAcceptConnection(m_clients[slot].getPlayerId());
				//	pending.sendGameData(m_world);
				//} else {
				//	pending.sendRejectConnection("Game Full");
				//}
			}
			it = m_pendingConnections.erase(it);
		}
		break;
	}
}
//
//void ServerEngine::stop() {
//	m_isServerRunning = false;
//	if (m_isServerRunning) {
//
//		broadcastServerShutdown();
//
//		enet_host_flush(m_host.handle);
//		for (auto& session : m_clients) {
//			session.disconnect();
//		}
//	}
//	if (m_serverThread.joinable()) {
//		m_serverThread.join();
//	}
//}
//
//void Server::BroadcastPacket(ServerPacket& packet, int channel, int flags) {
//	for (auto& client : m_clients) {
//		if (client.isActive())
//			client.sendPacket(packet, channel, flags);
//	}
//}
//
////std::vector<PendingClientSession>::iterator Server::FindPendingSession(u32 peerId) {
////	for (auto itr = m_pendingConnections.begin(); itr != m_pendingConnections.end();) {
////		auto thepeer = itr->connection.peer;
////		if (thepeer->incomingPeerID == peerId) {
////			return itr;
////		}
////	}
////	return m_pendingConnections.end();
////}
//
//int Server::CreateClientSession(ENetPeer* peer, u32 salt) {
//	for (unsigned i = 0; i < m_clients.size(); i++) {
//		if (!m_clients[i].isActive()) {
//			u32 playerId = m_world.addEntity();
//			m_clients[i].init(peer, salt, playerId);
//			m_clientsMap[peer->incomingPeerID] = i;
//			broadcastPlayerJoin(playerId);
//			return i;
//		}
//	}
//	return -1;
//}


void Server::Cleanup() {

}