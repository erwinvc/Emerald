#pragma once
#pragma pack(push, 1)

enum class PacketType : uint16 {
	Unk,
	Handshake,
	HandshakeChallenge,
	HandshakeResponse,
	ConnectionResult,
	ServerStatus,
	Kick,
	GameData,
	Chunk,
	AddEntity,
	RemoveEntity,
	UpdateEntities,
	BlockUpdate,
	PlayerState,
	Interaction,
	Time,
};

enum class HandshakeType : byte {
	Status,
	Login
};

class PacketWriter {
private:
	vector<byte> m_data;

public:
	PacketWriter(PacketType type) {
		Write<PacketType>(type);
	}
	template<typename T>
	void Write(const T& value) {
		m_data.insert(m_data.end(), ((byte*)&value), ((byte*)&value) + sizeof(T));
	}

	template<typename T>
	void Write(const T* data, uint32 count) {
		m_data.insert(m_data.end(), ((byte*)data), ((byte*)data) + (sizeof(T) * count));
	}

	uint32 GetSize() const {
		return m_data.size();
	}

	byte* GetData() const {
		return (byte*)m_data.data();
	}
};

class PacketReader {
private:
	const byte* m_data;
public:
	PacketType type;
	PacketReader(const void* data) {
		m_data = (const byte*)data;
		type = Read<PacketType>();
	}

	template<typename T>
	T Read() {
		T val = *(T*)m_data;
		m_data += sizeof(T);
		return val;
	}

	template<typename T>
	T* Read(uint32 count) {
		T* val = (T*)m_data;
		m_data += (sizeof(T) * count);
		return val;
	}
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

	const void* GetData() const {
		return m_data;
	}
};

struct PacketHandshake {
	PacketType type = PacketType::Handshake;
	HandshakeType handshakeType;
	uint32 salt;
	char name[16];

	void SetName(const String& str) {
		strcpy(name, str.c_str());
	}
};

struct PacketHandshakeChallenge {
	PacketType type = PacketType::HandshakeChallenge;
	uint32 salt;
};

struct PacketHandshakeResponse {
	PacketType type = PacketType::HandshakeResponse;
	uint32 salt;
};

struct PacketConnectionResult {
	PacketType type = PacketType::ConnectionResult;
	bool result; //0 = denied, 1 = accepted
	uint32 playerID;
	char reason[64];

	void SetReason(const String& str) {
		int size = str.size();
		strcpy(reason, str.c_str());
	}
};

struct PacketServerStatus {
	PacketType type = PacketType::ServerStatus;
	uint8 playerCount;
	uint8 maxPlayers;
	char serverTitle[64];
	char serverMotd[64];

	void SetTitle(const String& str) {
		int size = str.size();
		strcpy(serverTitle, str.c_str());
	}

	void SetMotd(const String& str) {
		int size = str.size();
		strcpy(serverMotd, str.c_str());
	}
};

struct PacketKick {
	PacketType type = PacketType::Kick;
	char reason[128];

	void SetReason(const String& str) {
		int size = str.size();
		strcpy(reason, str.c_str());
	}
};

struct PacketChunk {
	PacketType type = PacketType::Chunk;
	glm::ivec3 position;
	uint8 blocks[CHUNK_VOLUME];
};

struct PacketRemoveEntity {
	PacketType type = PacketType::RemoveEntity;
	uint32 entity;
};

struct PacketUpdateEntities {
	PacketType type = PacketType::UpdateEntities;

};

struct PacketBlockUpdate {
	PacketType type = PacketType::BlockUpdate;
	uint32 salt;
	BlockPos blockPosition;
	uint8 blockType;
};

struct PacketPlayerState {
	PacketType type = PacketType::PlayerState;
	uint32 salt;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 velocity;
};

struct PacketTime {
	PacketType type = PacketType::Time;
	uint64 time;
};

//struct PacketInteraction {
//	PacketType type = PacketType::Interaction;
//	BlockPos blockPosition;
//	uint8 block;
//};

#pragma pack(pop)