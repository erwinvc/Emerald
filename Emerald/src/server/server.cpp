#include "stdafx.h"

void Server::Initialize() {
	m_world = NEW(ServerWorld());
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

	Shutdown();
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
				OnPlayerDisconnect(evnt.peer);
				break;

			case NetEventType::Data: {
				ReceivedPacket packet(evnt.packet);
				HandlePacket(packet, evnt.peer);
				evnt.Destroy();
			} break;
		}
	}

	for (auto& client : m_clients) {
		if (client.IsActive()) {
			client.tick();
		}
	}

	BroadcastEntityStates();
}

void Server::Shutdown() {
	BroadcastServerShutdown();
	enet_host_flush(m_host.m_handle);
	for (auto& session : m_clients) {
		session.Disconnect();
	}
}

void Server::HandlePacket(const ReceivedPacket& packet, ENetPeer* peer) {
	switch (packet.type) {
		case PacketType::Handshake: OnHandshake(packet.As<PacketHandshake>(), peer); break;
		case PacketType::HandshakeResponse: OnHandshakeResponse(packet.As<PacketHandshakeResponse>(), peer); break;
		case PacketType::PlayerState: OnPlayerState(packet.As<PacketPlayerState>(), peer); break;
		case PacketType::BlockUpdate: OnBlockUpdate(packet.As<PacketBlockUpdate>(), peer); break;
	}
}

void Server::CreatePendingConnection(ENetPeer* peer) {
	LOG("[~cNetwork~x] new client connected %s", peer->address.host.u.Byte);
	PendingClientSession session;
	session.connection.m_peer = peer;
	m_pendingConnections.push_back(session);
}

uint32 Server::CreateClientSession(ENetPeer* peer, uint32 salt) {
	for (unsigned i = 0; i < m_clients.size(); i++) {
		if (!m_clients[i].IsActive()) {
			uint32 playerId = m_world->AddEntity();
			m_clients[i].Initialize(peer, salt, playerId);
			m_clientsMap[peer->incomingPeerID] = i;

			BroadcastPlayerJoin(playerId);
			return i;
		}
	}
	return -1;
}

void Server::OnHandshake(const PacketHandshake& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received %s handshake from client", packet.handshakeType == HandshakeType::Login ? "login" : "status");

	for (auto it = m_pendingConnections.begin(); it != m_pendingConnections.end(); it++) {
		auto& pending = *it;
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			if (packet.handshakeType == HandshakeType::Login) {
				pending.salt = packet.salt;
				PacketHandshakeChallenge newPacket;
				newPacket.salt = m_salt;
				pending.connection.SendPacket(&newPacket);
			} else {
				PacketServerStatus newPacket;
				newPacket.maxPlayers = 4;
				newPacket.playerCount = m_clients.size();
				newPacket.SetTitle("ServerTitle");
				newPacket.SetMotd("Motd!");
				pending.connection.SendPacket(&newPacket);

				it = m_pendingConnections.erase(it);
			}
			break;
		}
	}
}

void Server::OnHandshakeResponse(const PacketHandshakeResponse& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received handshake response from client");
	for (auto it = m_pendingConnections.begin(); it != m_pendingConnections.end(); it++) {
		auto& pending = *it;
		//LOG("%d %d", packet.salt, m_salt);
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			uint32 salt = pending.salt ^ m_salt;
			if (salt == packet.salt) {
				pending.salt = salt;
				LOG("[~cNetwork~x] client salt accepted!");
				int slot = CreateClientSession(peer, salt);
				if (slot != -1) {
					PacketConnectionResult newPacket;
					newPacket.result = true;
					newPacket.playerID = m_clients[slot].GetPlayerId();
					pending.connection.SendPacket(&newPacket);

					PacketWriter writer(PacketType::GameData);
					auto& chunks = m_world->GetChunks();
					writer.Write((uint32)chunks.size());
					for (auto& chunk : chunks) {
						writer.Write(chunk.first);
						for (int i = 0; i < CHUNK_VOLUME; i++) {
							writer.Write<uint8>(chunk.second.GetBlockAtIndex(i));
						}
					}

					m_world->WriteEntities(writer);
					pending.connection.SendPacketWriter(writer);
				} else {
					PacketConnectionResult newPacket;
					newPacket.result = false;
					newPacket.playerID = 0;
					newPacket.SetReason("Server is full!");
					pending.connection.SendPacket(&newPacket);
				}
			}
			it = m_pendingConnections.erase(it);
		}
		break;
	}
}

void Server::OnBlockUpdate(const PacketBlockUpdate& packet, ENetPeer* peer) {
	auto itr = m_clientsMap.find(peer->incomingPeerID);
	if (itr == m_clientsMap.end()) {
		return;
	}
	auto& client = m_clients.at(itr->second);
	if (!client.Verify(packet.salt)) {
		return;
	}

	m_world->SetBlock(packet.blockPosition, packet.blockType);
	BroadcastToPeers(m_host.m_handle, &packet, sizeof(PacketBlockUpdate), 0, ENET_PACKET_FLAG_RELIABLE);
}
void Server::OnPlayerState(const PacketPlayerState& packet, ENetPeer* peer) {
	auto itr = m_clientsMap.find(peer->incomingPeerID);
	if (itr == m_clientsMap.end()) {
		return;
	}
	auto& client = m_clients.at(itr->second);
	if (!client.Verify(packet.salt)) {
		return;
	}

	auto& player = m_world->GetEntity(client.GetPlayerId());
	player.lastPosition = player.position;
	player.lastRotation = player.rotation;

	player.position = packet.position;
	player.rotation = packet.rotation;
}

void Server::BroadcastEntityStates() {
	PacketWriter writer(PacketType::UpdateEntities);

	m_world->WriteEntities(writer);
	BroadcastPacket(writer);
}

void Server::BroadcastPlayerJoin(uint32 playerId) {
	LOG("[~cNetwork~x] broadcasted player join");

	PacketWriter writer(PacketType::AddEntity);
	writer.Write<uint32>(1);
	auto& player = m_world->GetEntity(playerId);
	writer.Write(playerId);
	writer.Write(player.position);
	writer.Write(player.rotation);

	BroadcastToPeers(m_host.m_handle, writer.GetData(), writer.GetSize(), 0, ENET_PACKET_FLAG_RELIABLE);
}

void Server::BroadcastPlayerLeave(uint32 playerId) {
	LOG("[~cNetwork~x] broadcasted player left");

	PacketRemoveEntity packet;
	packet.entity = playerId;
	BroadcastToPeers(m_host.m_handle, &packet, sizeof(PacketRemoveEntity), 0, ENET_PACKET_FLAG_RELIABLE);
}

void Server::BroadcastServerShutdown() {
	LOG("[~cNetwork~x] broadcasted server shutdown");

	PacketKick packet;
	packet.SetReason("Server shutdown");

	BroadcastToPeers(m_host.m_handle, &packet, sizeof(PacketKick), 0, ENET_PACKET_FLAG_RELIABLE);
}

void Server::OnPlayerDisconnect(ENetPeer* peer) {
	LOG("[~cNetwork~x] player disconnected");

	auto itr = m_clientsMap.find(peer->incomingPeerID);
	if (itr != m_clientsMap.end()) {
		auto index = itr->second;
		m_world->RemoveEntity(m_clients[index].GetPlayerId());
		m_clients[index].Disconnect();
		BroadcastPlayerLeave(m_clients[index].GetPlayerId());
		m_clientsMap.erase(itr);
	} else if (m_pendingConnections.size() > 0) {
		for (auto it = m_pendingConnections.begin(); it != m_pendingConnections.end(); it++) {
			auto& pending = *it;
			if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
				m_pendingConnections.erase(it);
			}
		}
	}
}

void Server::Cleanup() {
	DELETE(m_world);
}