#pragma once
#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include "json.h"

namespace emerald::jsonUtils {
	// Custom exceptions for better error handling
	class SerializationError : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	class DeserializationError : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	class FileError : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	class VersionMismatchError : public DeserializationError {
	public:
		VersionMismatchError(uint32_t expected, uint32_t actual)
			: DeserializationError(std::format("Version mismatch: expected {}, got {}", expected, actual)) {
		}
	};

	static nlohmann::json deserialize(const std::string_view& str) {
		try {
			return nlohmann::json::parse(str);
		} catch (const nlohmann::json::parse_error& e) {
			throw DeserializationError(std::format("Failed to parse JSON: {}", e.what()));
		}
	}

	static void saveToFile(const nlohmann::json j, const std::filesystem::path& path) {
		try {
			std::filesystem::create_directories(path.parent_path());
			std::ofstream file(path);
			if (!file) {
				throw FileError("Failed to open file for writing: " + path.string());
			}
			file << std::setw(2) << j;

			if (!file) {
				throw FileError("Failed to write to file: " + path.string());
			}
		} catch (const std::filesystem::filesystem_error& e) {
			throw FileError(std::string("Filesystem error: ") + e.what());
		}
	}

	static nlohmann::json readFromFile(const std::filesystem::path& path) {
		if (!std::filesystem::exists(path)) {
			throw FileError("File does not exist: " + path.string());
		}

		std::ifstream file(path);
		if (!file.is_open() || !file.good()) {
			throw FileError("Failed to open file: " + path.string());
		}
		
		nlohmann::json j;
		try {
			j = nlohmann::json::parse(file);
		} catch (std::exception ex) {
			Log::error("Error reading json file: {}", ex.what());
		}
		file.close();

		return j;
	}

	template<typename T>
	static T deserializeRequiredValue(const nlohmann::json& j, const std::string_view& key) {
		try {
			if (!j.contains(key)) {
				throw DeserializationError(std::format("Missing required field: {}", key));
			}
			return j.at(key).get<T>();
		} catch (const nlohmann::json::exception& e) {
			throw DeserializationError(std::format("Invalid value for field '{}':{}", key, e.what()));
		}
	}

	template<typename T>
	static std::optional<T> deserializeOptionalValue(const nlohmann::json& j, const std::string_view& key) {
		if (!j.contains(key)) {
			return std::nullopt;
		}
		try {
			return j.at(key).get<T>();
		} catch (const nlohmann::json::exception& e) {
			throw DeserializationError(std::format("Invalid value for optional field '{}': {}", key, e.what()));
		}
	}
}