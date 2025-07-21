#pragma once

namespace emerald {
	enum class ShaderType {
		Vertex,
		Fragment,
		Geometry,
		TessControl,
		TessEvaluation,
		Compute
	};

	inline ShaderType shaderTypeFromString(const std::string& str) {
		if (str == "vs")    return ShaderType::Vertex;
		if (str == "fs")    return ShaderType::Fragment;
		if (str == "gs")    return ShaderType::Geometry;
		if (str == "tcs")   return ShaderType::TessControl;
		if (str == "tes")   return ShaderType::TessEvaluation;
		if (str == "cs")    return ShaderType::Compute;
		throw std::runtime_error("Unknown shader type string: " + str);
	}

	struct UniformMetadata {
		std::string type;
		std::string name;
		std::string defaultValue;
		std::string editorAttributes;
	};

	struct ParsedShader {
		std::string programName;
		std::string vertexSource;
		std::string fragmentSource;
		std::string geometrySource;
		std::string tessControlSource;
		std::string tessEvalSource;
		std::string computeSource;

		std::vector<UniformMetadata> uniforms;
	};


	class ShaderParser {
	public:
		ShaderParser() = default;

		ParsedShader parseFile(const std::filesystem::path& filePath) {
			m_currentFilePath = filePath;
			m_parsedShader = ParsedShader{};

			std::ifstream file(filePath);
			if (!file.is_open()) {
				throw EmeraldError("[ShaderParser] cannot open file: {}", filePath.string());
			}

			m_lineNumber = 0;
			m_inShaderBlock = false;
			m_currentBlockType.clear();
			m_currentBlockContent.str(std::string());
			m_currentBlockContent.clear();

			std::string line;
			while (std::getline(file, line)) {
				m_lineNumber++;
				parseLine(line);
			}

			if (m_inShaderBlock) {
				finalizeBlock();
			}

			return m_parsedShader;
		}

	private:
		ParsedShader m_parsedShader;
		std::filesystem::path m_currentFilePath;
		int m_lineNumber = 0;

		bool m_inShaderBlock = false;     // Are we inside @vs/@fs/etc.?
		std::string m_currentBlockType;   // e.g. "vs", "fs", "gs" ...
		std::ostringstream m_currentBlockContent;

		void parseLine(const std::string& line) {
			std::string trimmed = trim(line);

			if (!trimmed.empty() && trimmed[0] == '@') {
				handleDirective(trimmed);
			} else {
				if (m_inShaderBlock) {
					m_currentBlockContent << line << "\n";
				}
			}
		}

		void handleDirective(const std::string& directiveLine) {
			std::vector<std::string> tokens = split(directiveLine, ' ');
			if (tokens.empty()) return;

			if (tokens[0] == "@program") {
				if (tokens.size() < 2) {
					error("Syntax error in @program directive. Usage: @program <name>", m_lineNumber);
				}
				m_parsedShader.programName = tokens[1];
			} else if (tokens[0] == "@vs" ||
				tokens[0] == "@fs" ||
				tokens[0] == "@gs" ||
				tokens[0] == "@tesc" ||
				tokens[0] == "@tese" ||
				tokens[0] == "@cs") {

				if (m_inShaderBlock) {
					error(std::format("{} found without while shader block is already open", tokens[0]), m_lineNumber);
				}
				m_inShaderBlock = true;
				m_currentBlockType = tokens[0].substr(1, tokens[0].size() - 1);

				if (m_currentBlockType == "vs" && !m_parsedShader.vertexSource.empty()) error("Duplicate vertex shader block", m_lineNumber);
				if (m_currentBlockType == "fs" && !m_parsedShader.fragmentSource.empty()) error("Duplicate fragment shader block", m_lineNumber);
				if (m_currentBlockType == "gs" && !m_parsedShader.geometrySource.empty()) error("Duplicate geometry shader block", m_lineNumber);
				if (m_currentBlockType == "ts" && !m_parsedShader.tessControlSource.empty()) error("Duplicate tessellation control shader block", m_lineNumber);
				if (m_currentBlockType == "te" && !m_parsedShader.tessEvalSource.empty()) error("Duplicate tessellation evaluation shader block", m_lineNumber);

				m_currentBlockContent.str(std::string());
				m_currentBlockContent.clear();
			} else if (tokens[0] == "@end") {
				if (!m_inShaderBlock) {
					error("@end found without an open shader block", m_lineNumber);
				}
				finalizeBlock();
			} else if (tokens[0] == "@include") {
				handleInclude(directiveLine);
			} else if (tokens[0] == "@uniform") {
				parseUniform(directiveLine);
			} else {
				error("Unknown directive: " + tokens[0], m_lineNumber);
			}
		}

		void finalizeBlock() {
			m_inShaderBlock = false;
			// Store the collected block content in the appropriate field
			if (m_currentBlockType == "vs") {
				m_parsedShader.vertexSource = m_currentBlockContent.str();
			} else if (m_currentBlockType == "fs") {
				m_parsedShader.fragmentSource = m_currentBlockContent.str();
			} else if (m_currentBlockType == "gs") {
				m_parsedShader.geometrySource = m_currentBlockContent.str();
			} else if (m_currentBlockType == "tesc") {
				m_parsedShader.tessControlSource = m_currentBlockContent.str();
			} else if (m_currentBlockType == "tese") {
				m_parsedShader.tessEvalSource = m_currentBlockContent.str();
			} else if (m_currentBlockType == "cs") {
				m_parsedShader.computeSource = m_currentBlockContent.str();
			} else {
				error("Unknown shader block type: " + m_currentBlockType, m_lineNumber);
			}
			m_currentBlockType.clear();
			m_currentBlockContent.str(std::string());
			m_currentBlockContent.clear();
		}

		void handleInclude(const std::string& directiveLine) {
			// Basic approach: find the string in quotes
			// e.g. @include "path/to/file.incl"
			size_t firstQuote = directiveLine.find('"');
			size_t secondQuote = directiveLine.find('"', firstQuote + 1);
			if (firstQuote == std::string::npos || secondQuote == std::string::npos) {
				error("Syntax error in @include directive. Expected quotes around path.", m_lineNumber);
			}
			std::string includePath = directiveLine.substr(firstQuote + 1, secondQuote - (firstQuote + 1));

			// Load the included file
			std::ifstream incFile(includePath);
			if (!incFile.is_open()) {
				error("Failed to open include file: " + includePath, m_lineNumber);
			}

			// Read its contents line by line and parse them as if they are part of the current file
			int oldLineNumber = m_lineNumber; // store for restoration after include
			std::filesystem::path oldFilePath = m_currentFilePath;

			m_currentFilePath = includePath;
			std::string line;
			while (std::getline(incFile, line)) {
				m_lineNumber++;
				parseLine(line);
			}

			// Restore old file path and line number
			m_currentFilePath = oldFilePath;
			m_lineNumber = oldLineNumber;
		}

		void parseUniform(const std::string& directiveLine) {
			// Remove "@uniform " prefix:
			const std::string prefix = "@uniform";
			std::string uniformLine = directiveLine.substr(prefix.size());
			uniformLine = trim(uniformLine);

			// Example uniform lines:
			// @uniform sampler2D _Albedo = (white);
			// @uniform float _Metallic = (0.5f) [range(0, 1)];

			// 1) Identify the type (sampler2D, float, vec3, etc.)
			// 2) Identify the name (e.g. _Metallic)
			// 3) Identify = (...) default
			// 4) Identify [someAttribute(...)] or [range(0,1)] etc.

			// Find the first space => "type"
			size_t spacePos = uniformLine.find(' ');
			if (spacePos == std::string::npos) {
				error("Uniform definition missing type/name.", m_lineNumber);
			}
			std::string type = uniformLine.substr(0, spacePos);
			std::string remaining = trim(uniformLine.substr(spacePos));

			// Next token => "name"
			spacePos = remaining.find(' ');
			if (spacePos == std::string::npos) {
				error("Uniform definition missing name.", m_lineNumber);
			}
			std::string name = remaining.substr(0, spacePos);
			remaining = trim(remaining.substr(spacePos));

			// Check for "= ( ... )"
			std::string defaultVal;
			if (!remaining.empty() && remaining[0] == '=') {
				// Remove '='
				remaining = trim(remaining.substr(1));

				// Expect something like (white), (0.5f), etc.
				if (!remaining.empty() && remaining[0] == '(') {
					size_t closingParen = remaining.find(')');
					if (closingParen == std::string::npos) {
						error("Missing closing parenthesis in uniform default value.", m_lineNumber);
					}
					defaultVal = remaining.substr(0, closingParen + 1); // e.g. "(white)"
					remaining = trim(remaining.substr(closingParen + 1));
				}
			}

			// Now check for optional attributes e.g. [range(0,1)] or [hideInInspector]
			std::string attributes;
			if (!remaining.empty() && remaining[0] == '[') {
				size_t closingBracket = remaining.find(']');
				if (closingBracket == std::string::npos) {
					error("Missing closing bracket for uniform attributes.", m_lineNumber);
				}
				attributes = remaining.substr(0, closingBracket + 1); // e.g. "[range(0,1)]"
				remaining = trim(remaining.substr(closingBracket + 1));
			}

			// Trim trailing semicolon if present
			if (!remaining.empty() && remaining.back() == ';') {
				remaining.pop_back();
			}

			// Store uniform data (for your editor/engine)
			UniformMetadata uniform;
			uniform.type = type;                 // e.g. "sampler2D" or "float"
			uniform.name = name;                 // e.g. "_Albedo"
			uniform.defaultValue = defaultVal;   // e.g. "(white)"
			uniform.editorAttributes = attributes;// e.g. "[range(0,1)]", "[hideInInspector]", etc.

			m_parsedShader.uniforms.push_back(uniform);

			// --  Insert a proper GLSL uniform line into the current shader code  --
			// For example: "uniform sampler2D _Albedo;"
			if (m_inShaderBlock) {
				m_currentBlockContent
					<< "uniform " << type << " " << name << ";" << "\n";

			} else {
				error("Uniform found outside of a shader block (e.g. @vs ... @end).", m_lineNumber);
			}
		}

		// Utility to throw an error with line context
		void error(const std::string& msg, int line) {
			std::ostringstream oss;
			oss << "[ShaderParser] error in file '" << m_currentFilePath.string()
				<< "' at line " << line << ": " << msg;
			throw EmeraldError(oss.str());
		}

		// Utility: split by delimiter
		std::vector<std::string> split(const std::string& s, char delim) {
			std::vector<std::string> elems;
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				std::string t = trim(item);
				if (!t.empty()) elems.push_back(t);
			}
			return elems;
		}

		// Utility: trim whitespace
		std::string trim(const std::string& s) {
			if (s.empty()) return s;
			size_t start = 0;
			while (start < s.size() && std::isspace((unsigned char)s[start])) start++;
			size_t end = s.size() - 1;
			while (end > start && std::isspace((unsigned char)s[end])) end--;
			return s.substr(start, end - start + 1);
		}
	};
}