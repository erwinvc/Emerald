#pragma once
#include "engine/ecs/core/entity.h"
#include "core/common/expected.h"
#include "utils/datastructures/asyncQueue.h"
#include "graphics/buffers/framebuffer.h"
#include <graphics/core/renderer.h>
#include <graphics/render/renderPass.h>

namespace emerald {
	class Picking {
		Picking();

		uint32_t registerObject(Entity e);
		void unregisterObject(Entity e);

		void requestRead(uint32_t  x, uint32_t y);
		void updateReadbacks();

		std::optional<Entity> getLastPick() const;

	private:
		struct ReadRequest { int x, y; uint32_t frame; };

		std::unordered_map<uint32_t, Entity> m_idToEntity;
		AsyncQueue<ReadRequest> m_pending;
		Ref<FrameBuffer> m_fbo;
		Ref<Shader> m_shader;
		Ref<RenderPass> m_renderPass;
	};
}