#pragma once
#include <stdexcept>
#include <string>

namespace emerald {
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

	template<typename Derived>
	class JsonSerializable {
	public:
		virtual ~JsonSerializable() = default;

		nlohmann::json toJson() const {
			return static_cast<const Derived*>(this)->toJsonImpl();
		}

		static Derived fromJson(const nlohmann::json& j) {
			return Derived::fromJsonImpl(j);
		}

		std::string serialize() const {
			try {
				return toJson().dump();
			} catch (const nlohmann::json::exception& e) {
				throw SerializationError(std::string("Failed to serialize: ") + e.what());
			}
		}

		static Derived deserialize(const std::string& str) {
			try {
				return fromJson(nlohmann::json::parse(str));
			} catch (const nlohmann::json::parse_error& e) {
				throw DeserializationError(std::string("Failed to parse JSON: ") + e.what());
			}
		}

		void serializeToFile(const std::filesystem::path& path) const {
			try {
				std::filesystem::create_directories(path.parent_path());
				std::ofstream file(path);
				if (!file) {
					throw FileError("Failed to open file for writing: " + path.string());
				}
				file << std::setw(4) << serialize();

				if (!file) {
					throw FileError("Failed to write to file: " + path.string());
				}
			} catch (const std::filesystem::filesystem_error& e) {
				throw FileError(std::string("Filesystem error: ") + e.what());
			}
		}

		static Derived deserializeFromFile(const std::filesystem::path& path) {
			try {
				if (!std::filesystem::exists(path)) {
					throw FileError("File does not exist: " + path.string());
				}

				std::ifstream file(path);
				if (!file) {
					throw FileError("Failed to open file for reading: " + path.string());
				}

				std::string content;
				content.assign(
					std::istreambuf_iterator<char>(file),
					std::istreambuf_iterator<char>()
				);

				if (!file) {
					throw FileError("Failed to read from file: " + path.string());
				}

				return deserialize(content);

			} catch (const std::filesystem::filesystem_error& e) {
				throw FileError(std::string("Filesystem error: ") + e.what());
			}
		}

	protected:
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
	};

}