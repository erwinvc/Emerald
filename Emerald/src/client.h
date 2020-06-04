#pragma once

enum class ConnectionState {
	Pending,
	Connected,
	Disconnected,
};

class Client : public Singleton<Client> {
private:
	Client() : m_updateTimer(UpdateTimer(60.0f)) { m_salt = Random::Int(0, Math::MAX_INT); }
	~Client() {}
	friend Singleton;

	ConnectionState m_connectionState = ConnectionState::Disconnected;
	Connection m_serverConnection;
	NetHost m_host;
	Window* m_window;
	NetHandlerClient* m_netHandler;
	bool m_initialized = false;
	Timer m_timer;
	UpdateTimer m_updateTimer;

	uint32 m_playerID;
	uint32 m_salt;
	float m_totalFrameTime = 0;
	uint64 m_frameCount = 0;
	float m_lastFrameTime = 0;
	int m_fps = 0;

	bool m_running = true;

	AsyncQueue<function<void()>> m_queue;

	void HandleQueue();
public:
	float globalValue = 0;

	ClientWorld* m_world;
	bool m_lockedMouse = false;
	HDRPipeline* pipeline = nullptr;


	void Initialize();
	void Run();
	void Cleanup();

	Window* GetWindow() { return m_window; }

	void Update(TimeStep time);
	void Render(float partialUpdate);

	void ConnectToServer(const String& IP, String_t name, int port);
	void Disconnect();

	uint64_t GetFrameCount() { return m_frameCount; }

	uint GetWidth() { return m_window->GetWidth(); }
	uint GetHeight() { return m_window->GetHeight(); }
	float GetAspect() { return m_window->GetAspect(); }


	ConnectionState GetConnectionState() { return m_connectionState; }

	void SetBlock(const BlockPos& pos, uint8 block);

	uint32 GetPlayerID() { return m_playerID; }

	float GetRealTime() { return m_timer.Get(); }
	float GetRealTimeSeconds() { return m_timer.Get(Timer::TimeFormat::SECONDS); }
	float GetTotalFrameTime() { return m_totalFrameTime; }

	inline void QueueTask(function<void()> task) { m_queue.Add(task); }

	friend NetHandlerClient;
};

inline Client* GetClient() { return Client::GetInstance(); }