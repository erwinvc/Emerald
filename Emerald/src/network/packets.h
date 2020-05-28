#pragma once
#pragma pack(push, 1)

enum class PacketType : uint16 {
	Unk,
	HandshakeOne,
	HandshakeChallenge,
	HandshakeResponse,
	ConnectionAccepted,
	Kick,
	GameData,
	AddEntity,
	RemoveEntity,
	UpdateEntities,
	BlockUpdate
};

struct Packet {
	PacketType type = PacketType::Unk;
};

class ReceivedPacket {
private:
	const void* m_data;
public:
	PacketType type;
	ReceivedPacket(ENetPacket* data) {
		type = (*(Packet*)data->data).type;
		m_data = data->data;
	}

	template<typename T>
	T& As() const {
		return *(T*)m_data;
	}
};

struct PacketHandshakeOne {
	PacketType type = PacketType::HandshakeOne;
	uint32 salt;
};

struct PacketHandshakeChallenge {
	PacketType type = PacketType::HandshakeChallenge;
	uint32 salt;
};

struct PacketHandshakeResponse {
	PacketType type = PacketType::HandshakeResponse;
	uint32 salt;
};

struct PacketConnectionAccepted {
	PacketType type = PacketType::ConnectionAccepted;

};

struct PacketKick {
	PacketType type = PacketType::Kick;

};

struct PacketGameData {
	PacketType type = PacketType::GameData;

};

struct PacketAddEntity {
	PacketType type = PacketType::AddEntity;

};

struct PacketRemoveEntity {
	PacketType type = PacketType::RemoveEntity;

};

struct PacketUpdateEntities {
	PacketType type = PacketType::UpdateEntities;

};

struct PacketBlockUpdate {
	PacketType type = PacketType::BlockUpdate;

};

#pragma pack(pop)