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
		VersionMismatchError(int expected, int actual)
			: DeserializationError("Version mismatch: expected " + std::to_string(expected) +
				", got " + std::to_string(actual)) {
		}
	};

	static nlohmann::json deserialize(const std::string& str) {
		try {
			return nlohmann::json::parse(str);
		} catch (const nlohmann::json::parse_error& e) {
			throw DeserializationError(std::string("Failed to parse JSON: ") + e.what());
		}
	}

	static void saveToFile(const nlohmann::json j, const std::filesystem::path& path) {
		try {
			std::filesystem::create_directories(path.parent_path());
			std::ofstream file(path);
			if (!file) {
				throw FileError("Failed to open file for writing: " + path.string());
			}
			file << std::setw(4) << j;

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
		if (!file.is_open()) {
			throw FileError("Failed to open file: " + path.string());
		}

		return nlohmann::json::parse(file);
	}

	template<typename T>
	static T deserializeRequiredValue(const nlohmann::json& j, const std::string& key) {
		try {
			if (!j.contains(key)) {
				throw DeserializationError("Missing required field: " + key);
			}
			return j.at(key).get<T>();
		} catch (const nlohmann::json::exception& e) {
			throw DeserializationError("Invalid value for field '" + key + "': " + e.what());
		}
	}

	template<typename T>
	static std::optional<T> deserializeOptionalValue(const nlohmann::json& j, const std::string& key) {
		if (!j.contains(key)) {
			return std::nullopt;
		}
		try {
			return j.at(key).get<T>();
		} catch (const nlohmann::json::exception& e) {
			throw DeserializationError("Invalid value for optional field '" + key + "': " + e.what());
		}
	}
}