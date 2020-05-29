#pragma once

class Profiler;
enum class ProfilerDataType {
	Update,
	Render,
	GPUFrame,
	Geometry,
	AmbientOcclusion,
	SSR,
	EmissionAmbient,
	DirectionalLighting,
	PointLighting,
	UI,
	Bloom,
	DirectionalShadow,
	COUNT
};

class ProfilerQuery {
private:
	Profiler* m_profiler;
	ProfilerDataType m_dataType;
	bool m_active;
public:
	Timer m_timer;
	ProfilerQuery(bool active, ProfilerDataType type, Profiler* profiler) : m_active(active), m_dataType(type), m_timer(Timer()), m_profiler(profiler) {}

	void End();
	friend class Profiler;
};

struct GLProfilerQueryHandle {
	bool m_active;
	uint32 m_startHandle;

	void End();

	GLProfilerQueryHandle(bool active, uint32 handle) : m_active(active), m_startHandle(handle) {}
};

class Profiler : public Singleton<Profiler> {
private:
	struct GLProfilerQuery {
		ProfilerDataType m_dataType;

		GLuint m_startHandle;
		GLuint m_stopHandle;
	};

	String_t names[12] = {
		"Update",
		"Render",
		"GPUFrame",
		"Geometry",
		"AmbientOcclusion",
		"SSR",
		"EmissionAmbient",
		"DirectionalLighting",
		"PointLighting",
		"UI",
		"Bloom",
		"DirectionalShadow"
	};

	int TIMER = 0;
	bool shouldStart = false;

	float m_profilerData[(int)ProfilerDataType::COUNT] = { 0 };
	vector<GLProfilerQuery> m_querries;

	void EndProfilerQuery(ProfilerDataType type, float time) {
		m_profilerData[(int)type] = time;
	}

	void EndGLProfilerQuery(uint32 startHandle) {
		for (auto& it = m_querries.begin(); it != m_querries.end(); it++) {
			auto& query = *it;
			if (query.m_startHandle == startHandle) {
				glGenQueries(1, &query.m_stopHandle);
				GL(glQueryCounter(query.m_stopHandle, GL_TIMESTAMP));
			}
		}
	}

	friend ProfilerQuery;
	friend GLProfilerQueryHandle;
public:

	GLProfilerQueryHandle StartGL(ProfilerDataType type) {
		if (shouldStart) {
			m_querries.push_back(GLProfilerQuery());
			auto& query = m_querries.back();
			query.m_dataType = type;
			glGenQueries(1, &query.m_startHandle);
			GL(glQueryCounter(query.m_startHandle, GL_TIMESTAMP));
			return GLProfilerQueryHandle(true, query.m_startHandle);
		} else {
			return GLProfilerQueryHandle(false, 0);
		}
	}

	ProfilerQuery Start(ProfilerDataType type) {
		return { shouldStart, type, this };
	}

	void Update() {
		shouldStart = false;
		Utils::DoTimedFunction(&TIMER, 100, [&] {shouldStart = true; });
		if (m_querries.size() > 0) {
			auto it = m_querries.begin();
			while (it != m_querries.end()) {
				auto& query = *it;
				GLint available = 0;
				glGetQueryObjectiv(query.m_stopHandle, GL_QUERY_RESULT_AVAILABLE, &available);
				if (available) {
					GLuint64 startTime = 0;
					GLuint64 stopTime = 0;
					glGetQueryObjectui64v(query.m_startHandle, GL_QUERY_RESULT, &startTime);
					glGetQueryObjectui64v(query.m_stopHandle, GL_QUERY_RESULT, &stopTime);
					m_profilerData[(int)query.m_dataType] = (stopTime - startTime) / 1000000.0f;
					glDeleteQueries(1, &query.m_startHandle);
					glDeleteQueries(1, &query.m_stopHandle);
					it = m_querries.erase(it);
				} else it++;
			}
		}
	}

	void OnImGui() {
		if (ImGui::BeginTabItem("Profiler")) {
			UI::Begin();
			for (int i = 0; i < (int)ProfilerDataType::COUNT; i++) {
				UI::Text(names[i], Format("%.4fms", m_profilerData[i]));
			}
			UI::End();
			ImGui::EndTabItem();
		}
	}
};

inline Profiler* GetProfiler() { return Profiler::GetInstance(); }