#pragma once
#include "utils/datastructures/vector.h"
#include "engine/ecs/core/entity.h"
#include "graphics/buffers/framebuffer.h"
#include "utils/datastructures/asyncQueue.h"
#include "graphics/render/renderPass.h"

namespace emerald {
	class Picking {
	public:
		struct ReadResult {
			uint32_t x, y;
			Entity entity;
		};

		Picking();
		void requestRead(glm::ivec2 pos, int radius);
		void render();
		const std::vector<Picking::ReadResult>& flushResults();

		Ref<FrameBuffer> getFBO() {
			return m_fbo;
		}

	private:
		struct ReadRequest {
			uint32_t x, y;
			uint32_t w, h;
			uint64_t frame;
		};
		const FBOScale m_fboScale = FBOScale::FULL;
		Vector<Entity> m_pickingTable;
		std::vector<ReadResult> m_completed;   
		AsyncQueue<ReadRequest> m_pending;
		Ref<FrameBuffer> m_fbo;
		Ref<Shader> m_shader;
		Ref<Material> m_material;
		Ref<RenderPass> m_renderPass;
	};
}