#include "eepch.h"
#include "picking.h"
#include "engine\ecs\components\transformComponent.h"
#include "engine\ecs\components\meshRendererComponent.h"
#include "..\..\editor\src\core\editor.h"
#include "imguiProfiler\Profiler.h"

namespace emerald {
	Picking::Picking() {
		FramebufferDesc desc;
		desc.samples = MSAA::NONE;
		desc.scale = m_fboScale;
		desc.attachments = { {"ID", TextureFormat::R32UI }, { "Debug", TextureFormat::RGBA8  }, {"Depth", TextureFormat::DEPTH32F} };
		desc.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		desc.name = "Picking";
		m_fbo = FrameBuffer::create(desc);

		m_shader = ShaderRegistry::getShader("ID");
		m_material = Ref<Material>::create("ID", m_shader);

		RenderPassDesc passDesc;
		passDesc.name = "Picking Pass";
		passDesc.frameBuffer = m_fbo;
		passDesc.shader = m_shader;
		passDesc.clearMask.setFlags(ClearMaskFlags::COLOR, ClearMaskFlags::DEPTH);
		m_renderPass = Ref<RenderPass>::create(passDesc);
	}

	void Picking::requestRead(glm::ivec2 pos, int radius) {
		float s = FrameBuffer::fboScaleToFloat(m_fboScale);
		ReadRequest req{
			uint32_t((pos.x - radius) * s),
			uint32_t((pos.y - radius) * s),
			1 + radius * 2,
			1 + radius * 2,
			App->getFrameCount()
		};
		m_pending.push(req);
	}

	const std::vector<Picking::ReadResult>& Picking::flushResults() {
		PROFILE_GPU_SCOPE("PickingFlush");
		m_completed.clear();

		m_fbo->bindRead();
		while (!m_pending.empty()) {
			ReadRequest req;
			m_pending.peek(req);

			if (req.frame + 1 > App->getFrameCount()) break;

			std::vector<uint32_t> ids(req.w * req.h);
			GL(glReadPixels(req.x, req.y, req.w, req.h,
				GL_RED_INTEGER, GL_UNSIGNED_INT, ids.data()));

			std::unordered_set<uint32_t> uniqueIDs;
			for (uint32_t id : ids) if (id) uniqueIDs.insert(id);

			struct Hit { Entity e; float depth; };
			std::vector<Hit> hits;
			hits.reserve(uniqueIDs.size());

			Ref<Camera> cam = Editor->getEditorCamera();
			for (uint32_t id : uniqueIDs) {
				if (id - 1 >= m_pickingTable.size()) continue;
				Entity  e = m_pickingTable[id - 1];

				auto* tr = e.getComponent<TransformComponent>();
				if (!tr) continue;

				float dz = glm::distance(cam->getPosition(), tr->getGlobalPosition());
				hits.push_back({ e, dz });
			}
			std::sort(hits.begin(), hits.end(),
				[](const Hit& a, const Hit& b) { return a.depth < b.depth; });

			for (const auto& h : hits)
				m_completed.push_back({ req.x, req.y, h.e });

			m_pending.pop();
		}
		FrameBufferManager::bindDefaultFBO();

		return m_completed;
	}

	void Picking::render() {
		PROFILE_GPU_SCOPE("PickingPass");
		m_pickingTable.clear();
		uint32_t id = 1;

		Renderer::beginRenderPass(m_renderPass);
		m_renderPass->clear();

		auto view = EntityComponentSystem::View<MeshRendererComponent, TransformComponent>();

		m_material->updateForRendering();
		m_material->set("_ViewMatrix", Editor->getEditorCamera()->getViewMatrix());
		m_material->set("_ProjectionMatrix", Editor->getEditorCamera()->getProjectionMatrix());
		for (auto [meshRenderer, transform] : view) {
			m_pickingTable.pushBack(meshRenderer->getEntity());

			Ref<Model> model = meshRenderer->m_model.get();
			if (!model) continue;

			Ref<Mesh> submesh = model->getSubMesh(meshRenderer->m_submeshIndex);
			if (!submesh)continue;

			m_material->set("_ModelMatrix", transform->getGlobalTransform());
			m_material->set("_ID", id);
			m_material->updateForRendering();

			submesh->bind();
			Renderer::drawIndexed(submesh->getIBO()->getCount(), PrimitiveType::TRIANGLES);

			++id;
		}
		Renderer::endRenderPass();
	}
}