#pragma once

class JsonWrapper {
private:
	nlohmann::json m_json;
public:
	JsonWrapper(nlohmann::json& jsonOb) : m_json(jsonOb) {
	}

	operator=(nlohmann::json jsonOb) {
		m_json = jsonOb;
	}

	JsonWrapper operator[] (const String& str) {
		return jsonOb[str];
	}

	template <typename T>
	T Get(const string &key, T defval = T()) {
		if (m_json.is_object()) {
			auto it = m_json.find(key);
			if (it != m_json.end()) {
				try {
					return it.value().get<T>();
				} catch (...) {

				}
			}
		}
		return defval;
	}
};