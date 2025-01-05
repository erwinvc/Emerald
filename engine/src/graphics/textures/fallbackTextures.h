#pragma once
#include "texture.h"

namespace emerald {
	class FallbackTextures {
	public:
		static void initialize();
		static inline Ref<Texture> null() { return s_null; }
		static inline Ref<Texture> white() { return s_white; }
		static inline Ref<Texture> black() { return s_black; }
		static inline Ref<Texture> normal() { return s_normal; }
		static inline Ref<Texture> transparent() { return s_transparent; }

	private:
		static inline Ref<Texture> s_null;
		static inline Ref<Texture> s_white;
		static inline Ref<Texture> s_black;
		static inline Ref<Texture> s_normal;
		static inline Ref<Texture> s_transparent;
	};
}