#include "eepch.h"
#include "fallbackTextures.h"
#include "utils/math/color.h"

namespace emerald {
	void FallbackTextures::initialize() {
		TextureDesc desc;
		desc.format = TextureFormat::RGBA8;
		desc.filter = TextureFilter::NEAREST;
		desc.wrap = TextureWrap::REPEAT;

		desc.name = "Fallback_white";
		uint32_t white = Color::white().toRGBA8();
		s_white = Ref<Texture>::create(desc, 1, 1, (byte*)&white, (uint32_t)sizeof(uint32_t));
		s_white->submitInvalidate();

		desc.name = "Fallback_black";
		uint32_t black = Color::black().toRGBA8();
		s_black = Ref<Texture>::create(desc, 1, 1, (byte*)&black, (uint32_t)sizeof(uint32_t));
		s_black->submitInvalidate();

		desc.name = "Fallback_normal";
		uint32_t normal = Color::normalMap().toRGBA8();
		s_normal = Ref<Texture>::create(desc, 1, 1, (byte*)&normal, (uint32_t)sizeof(uint32_t));
		s_normal->submitInvalidate();

		desc.name = "Fallback_transparent";
		uint32_t transparent = Color::transparent().toRGBA8();
		s_transparent = Ref<Texture>::create(desc, 1, 1, (byte*)&transparent, (uint32_t)sizeof(uint32_t));
		s_transparent->submitInvalidate();

		uint32_t nullColors[] = {
			Color::black().toRGBA8(),
			Color::magenta().toRGBA8(),
			Color::magenta().toRGBA8(),
			Color::black().toRGBA8()
		};

		desc.name = "Fallback_null";
		s_null = Ref<Texture>::create(desc, 2, 2, (byte*)&nullColors, (uint32_t)sizeof(uint32_t) * 4);
		s_null->submitInvalidate();
	}
}