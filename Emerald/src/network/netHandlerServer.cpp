#include "stdafx.h"

void NetHandlerServer::HandlePacket(const ReceivedPacket& packet, ENetPeer* peer) {
	switch (packet.type) {
		case PacketType::Handshake: OnHandshake(packet.As<PacketHandshake>(), peer); break;
		case PacketType::HandshakeResponse: OnHandshakeResponse(packet.As<PacketHandshakeResponse>(), peer); break;
		case PacketType::PlayerState: OnPlayerState(packet.As<PacketPlayerState>(), peer); break;
		case PacketType::BlockUpdate: OnBlockUpdate(packet.As<PacketBlockUpdate>(), peer); break;
	}
}

void NetHandlerServer::OnBlockUpdate(const PacketBlockUpdate& packet, ENetPeer* peer) {
	auto itr = m_server->m_clientsMap.find(peer->incomingPeerID);
	if (itr == m_server->m_clientsMap.end()) {
		return;
	}
	auto& client = m_server->m_clients.at(itr->second);
	if (!client.Verify(packet.salt)) {
		return;
	}

	m_server->m_world->SetBlock(packet.blockPosition, packet.blockType);
	BroadcastToPeers(m_server->m_host.m_handle, &packet, sizeof(PacketBlockUpdate), 0, ENET_PACKET_FLAG_RELIABLE);
}
void NetHandlerServer::OnPlayerState(const PacketPlayerState& packet, ENetPeer* peer) {
	auto itr = m_server->m_clientsMap.find(peer->incomingPeerID);
	if (itr == m_server->m_clientsMap.end()) {
		return;
	}
	auto& client = m_server->m_clients.at(itr->second);
	if (!client.Verify(packet.salt)) {
		return;
	}

	auto& player = m_server->m_world->GetEntity(client.GetPlayerId());
	player.lastPosition = player.position;
	player.lastRotation = player.rotation;

	player.position = packet.position;
	player.rotation = packet.rotation;
	player.velocity = packet.velocity;
}


void NetHandlerServer::BroadcastTime(uint64 time) {
	PacketTime packet;
	packet.time = time;

	BroadcastToPeers(m_server->m_host.m_handle, &packet, sizeof(PacketTime), 0, ENET_PACKET_FLAG_RELIABLE);
}

void NetHandlerServer::BroadcastEntityStates() {
	PacketWriter writer(PacketType::UpdateEntities);

	m_server->m_world->WriteEntities(writer);
	m_server->BroadcastPacket(writer);
}

void NetHandlerServer::BroadcastPlayerJoin(uint32 playerId) {
	LOG("[~cNetwork~x] broadcasted player join");

	PacketWriter writer(PacketType::AddEntity);
	writer.Write<uint32>(1);
	auto& player = m_server->m_world->GetEntity(playerId);
	writer.Write(playerId);
	writer.Write(player.position);
	writer.Write<char>(player.name, 16);
	writer.Write(player.rotation);
	writer.Write(player.velocity);

	BroadcastToPeers(m_server->m_host.m_handle, writer.GetData(), writer.GetSize(), 0, ENET_PACKET_FLAG_RELIABLE);
}

void NetHandlerServer::BroadcastPlayerLeave(uint32 playerId) {
	LOG("[~cNetwork~x] broadcasted player left");

	PacketRemoveEntity packet;
	packet.entity = playerId;
	BroadcastToPeers(m_server->m_host.m_handle, &packet, sizeof(PacketRemoveEntity), 0, ENET_PACKET_FLAG_RELIABLE);
}

void NetHandlerServer::BroadcastServerShutdown() {
	LOG("[~cNetwork~x] broadcasted server shutdown");

	PacketKick packet;
	packet.SetReason("Server shutdown");

	BroadcastToPeers(m_server->m_host.m_handle, &packet, sizeof(PacketKick), 0, ENET_PACKET_FLAG_RELIABLE);
}

void NetHandlerServer::OnPlayerDisconnect(ENetPeer* peer) {
	auto itr = m_server->m_clientsMap.find(peer->incomingPeerID);
	if (itr != m_server->m_clientsMap.end()) {
		auto index = itr->second;
		m_server->m_world->RemoveEntity(m_server->m_clients[index].GetPlayerId());
		m_server->m_clients[index].Disconnect();
		BroadcastPlayerLeave(m_server->m_clients[index].GetPlayerId());
		m_server->m_clientsMap.erase(itr);
		LOG("[~cNetwork~x] player disconnected");
	} else if (m_server->m_pendingConnections.size() > 0) {
		auto it = m_server->m_pendingConnections.begin();
		while (it != m_server->m_pendingConnections.end()) {
			auto& pending = *it;
			if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
				it = m_server->m_pendingConnections.erase(it);
			} else it++;
		}
	}
}

void NetHandlerServer::OnHandshake(const PacketHandshake& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received %s handshake from client %s", packet.handshakeType == HandshakeType::Login ? "login" : "status", packet.handshakeType == HandshakeType::Login ? packet.name : "");

	for (auto it = m_server->m_pendingConnections.begin(); it != m_server->m_pendingConnections.end(); it++) {
		auto& pending = *it;
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			if (packet.handshakeType == HandshakeType::Login) {
				pending.salt = packet.salt;
				strcpy(pending.name, packet.name);
				PacketHandshakeChallenge newPacket;
				newPacket.salt = m_server->m_salt;
				pending.connection.SendPacket(&newPacket);
			} else {
				PacketServerStatus newPacket;
				newPacket.maxPlayers = 4;
				newPacket.playerCount = m_server->m_clients.size();
				newPacket.SetTitle("ServerTitle");
				newPacket.SetMotd("Motd!");
				pending.connection.SendPacket(&newPacket);

				it = m_server->m_pendingConnections.erase(it);
			}
			break;
		}
	}
}

void NetHandlerServer::OnHandshakeResponse(const PacketHandshakeResponse& packet, ENetPeer* peer) {
	LOG("[~cNetwork~x] received handshake response from client %s", peer->address.host.u.Byte);
	for (auto it = m_server->m_pendingConnections.begin(); it != m_server->m_pendingConnections.end(); it++) {
		auto& pending = *it;
		if (pending.connection.m_peer->incomingPeerID == peer->incomingPeerID) {
			uint32 salt = pending.salt ^ m_server->m_salt;
			if (salt == packet.salt) {
				pending.salt = salt;
				LOG("[~cNetwork~x] client %s salt accepted!", pending.name);
				int slot = m_server->CreateClientSession(peer, pending.name, salt);
				if (slot != -1) {
					PacketConnectionResult connectionResult;
					connectionResult.result = true;
					connectionResult.playerID = m_server->m_clients[slot].GetPlayerId();
					pending.connection.SendPacket(&connectionResult);

					PacketWriter gameData(PacketType::GameData);
					auto& chunks = m_server->m_world->GetChunks();
					
					gameData.Write((uint32)chunks.size());
					for (auto& chunk : chunks) {
						gameData.Write(chunk.first);
						chunk.second.Compress(gameData);
					}
					m_server->m_world->WriteEntities(gameData);

					pending.connection.SendPacketWriter(gameData);
				} else {
					PacketConnectionResult connectionResult;
					connectionResult.result = false;
					connectionResult.playerID = 0;
					connectionResult.SetReason("Server is full!");
					pending.connection.SendPacket(&connectionResult);
				}
			}
			it = m_server->m_pendingConnections.erase(it);
		}
		break;
	}
}