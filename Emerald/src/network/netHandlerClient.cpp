#include "stdafx.h"

void NetHandlerClient::HandlePacket(const ReceivedPacket& packet) {
	switch (packet.type) {
		case PacketType::HandshakeChallenge: OnHandshakeChallenge(packet.As<PacketHandshakeChallenge>()); break;
		case PacketType::ConnectionResult: OnConnectionResult(packet.As<PacketConnectionResult>()); break;
		case PacketType::Kick: OnKick(packet.As<PacketKick>()); break;
		case PacketType::GameData: OnGameData(packet.GetData()); break;
		case PacketType::AddEntity: OnAddEntity(packet.GetData()); break;
		case PacketType::RemoveEntity: OnRemoveEntity(packet.As<PacketRemoveEntity>()); break;
		case PacketType::UpdateEntities: OnUpdateEntities(packet.GetData()); break;
		case PacketType::BlockUpdate: OnBlockUpdate(packet.As<PacketBlockUpdate>()); break;
		case PacketType::Time: OnTime(packet.As<PacketTime>()); break;
	}
}

void NetHandlerClient::OnHandshakeChallenge(const PacketHandshakeChallenge& packet) {
	uint32 newSalt = m_client->m_salt ^ packet.salt;
	m_client->m_salt = newSalt;

	PacketHandshakeResponse newPacket;
	newPacket.salt = m_client->m_salt;
	m_client->m_serverConnection.SendPacket(&newPacket);

	LOG("[~cNetwork~x] received handshake challenge");
	m_client->m_connectionState = ConnectionState::Connected;
}

void NetHandlerClient::OnConnectionResult(const PacketConnectionResult& packet) {
	if (packet.result) {
		LOG("[~cNetwork~x] connection accepted");
		m_client->m_connectionState = ConnectionState::Connected;
		m_client->m_playerID = packet.playerID;
		m_client->m_world = NEW(ClientWorld());
	} else {
		LOG("[~cNetwork~x] connection declined: %s", packet.reason);
		m_client->Disconnect();
	}
}

void NetHandlerClient::OnKick(const PacketKick& packet) {
	LOG("[~cNetwork~x] kicked! %s", packet.reason);
	GetClient()->Disconnect();
	GameStates::VOXEL->GetWorld()->Clear();
	GetStateManager()->SetState(GameStates::MENU);
}

void NetHandlerClient::OnGameData(const void* data) {
	LOG("[~cNetwork~x] received chunk data");

	PacketReader reader(data);
	PacketType type = reader.type;
	uint32 chunkCount = reader.Read<uint32>();

	Timer timer;
	for (uint32 i = 0; i < chunkCount; i++) {
		ClientWorld* world = GameStates::VOXEL->GetWorld();
		glm::ivec3 pos = reader.Read<glm::ivec3>();
		world->m_chunks.emplace(pos, Chunk(pos, reader));
	}

	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 id = reader.Read<uint32>();
		auto position = reader.Read<glm::vec3>();
		auto rotation = reader.Read<glm::vec3>();
		GameStates::VOXEL->GetWorld()->AddEntity(id, position, rotation);
	}

	LOG("[~cNetwork~x] parsing %u chunks took %.2fms", chunkCount, timer.Get());

	GetStateManager()->SetState(GameStates::VOXEL);
}

void NetHandlerClient::OnAddEntity(const void* data) {
	LOG("[~cNetwork~x] added entity");

	PacketReader reader(data);
	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 entityId = reader.Read<uint32>();
		glm::vec3 position = reader.Read<glm::vec3>();
		glm::vec3 rotation = reader.Read<glm::vec3>();

		GameStates::VOXEL->GetWorld()->AddEntity(entityId, position, rotation);
	}
}

void NetHandlerClient::OnRemoveEntity(const PacketRemoveEntity& packet) {
	LOG("[~cNetwork~x] removed entity");

	GameStates::VOXEL->GetWorld()->RemoveEntity(packet.entity);
}

void NetHandlerClient::OnUpdateEntities(const void* data) {
	PacketReader reader(data);
	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 entityId = reader.Read<uint32>();
		glm::vec3 position = reader.Read<glm::vec3>();
		glm::vec3 rotation = reader.Read<glm::vec3>();
		GameStates::VOXEL->GetWorld()->UpdateEntity(entityId, position, rotation);
	}
}

void NetHandlerClient::OnBlockUpdate(const PacketBlockUpdate& packet) {
	GameStates::VOXEL->GetWorld()->SetBlock(packet.blockPosition, packet.blockType);
}
void NetHandlerClient::OnTime(const PacketTime& packet) {
	m_client->m_world->SetTime(packet.time);
}