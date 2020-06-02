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
	ClientWorld* m_world;
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
	HDRPipeline* pipeline = nullptr;

	float GetTotalFrameTime() { return m_totalFrameTime; }

	void Initialize();
	void Run();
	void Cleanup();

	void Update(TimeStep time);
	void Render(float partialUpdate);

	void ConnectToServer(const String& IP, int port);
	void Disconnect();

	void QueueTask(function<void()> task) {
		m_queue.Add(task);
	}
	uint64_t GetFrameCount() { return m_frameCount; }

	uint GetWidth() { return m_window->GetWidth(); }
	uint GetHeight() { return m_window->GetHeight(); }
	float GetAspect() { return m_window->GetAspect(); }

	Window* GetWindow() { return m_window; }

	ConnectionState GetConnectionState() { return m_connectionState; }

	void SetBlock(const BlockPos& pos, uint8 block);

	uint32 GetPlayerID() { return m_playerID; }

	float GetRealTime() { return m_timer.Get(); }
	float GetRealTimeSeconds() { return m_timer.Get(Timer::TimeFormat::SECONDS); }

	friend NetHandlerClient;
};

static Client* GetClient() {
	return Client::GetInstance();
}

//static GraphicsPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }