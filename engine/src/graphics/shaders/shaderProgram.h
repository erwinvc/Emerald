#pragma once

namespace emerald {
	class ShaderProgram {
	private:
		uint32_t m_handle = 0xffffffff;
		uint32_t m_globalUniformsBlockIndex = 0xffffffff;
		std::vector<uint32_t> m_attachedShaders;

		void getGlobalUniformBlock();

	public:
		ShaderProgram() {}
		~ShaderProgram();

		void createProgram();
		void attachShader(uint32_t shader);
		void linkAndValidate();
		void deleteAttachedShaders();
		void bind();

		bool hasValidHandle() const { return m_handle != 0xffffffff; }
		uint32_t handle() const { return m_handle; }
	};
}
