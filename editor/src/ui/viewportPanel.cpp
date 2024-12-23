#include "eepch.h"
#include "viewportPanel.h"
#include "ui/imguiManager.h"
#include "editor.h"
#include "input/dragDrop.h"
#include "engine/assets/core/assetRegistry.h"
#include "engine/assets/model/model.h"
#include "engine/scene/sceneManager.h"
#include "engine/ecs/components/meshRendererComponent.h"
#include "engine/ecs/components/sceneGraphComponent.h"
#include "utils/threading/jobSystem.h"
#include "engine/assets/streaming/streaming.h"
#include "engine/events/events.h"

namespace emerald {
	ViewportPanel::ViewportPanel() {
	}

	void ViewportPanel::draw() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav;

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		if (ImGui::Begin("Viewport", nullptr, windowFlags)) {
			ImVec2 avail = ImGui::GetContentRegionAvail();

			ImGui::Image((void*)(uint64_t)Editor->getFinalTexture()->handle(), avail, { 0, 1 }, { 1, 0 });

			m_sceneViewportSize = glm::ivec2((uint32_t)avail.x, (uint32_t)avail.y);
			m_mouseInViewport = ImGui::IsWindowHovered();

			m_viewportFocused = ImGui::IsWindowFocused();

			if (m_mouseInViewport) {
				ImGui::SetNextFrameWantCaptureMouse(false);
			}

			if (m_viewportFocused) {
				ImGui::SetNextFrameWantCaptureKeyboard(false);
			}

			if (m_mouseInViewport && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::FocusWindow(ImGui::GetCurrentWindow());
			}

			handleDragDrop();

		} else {
			m_sceneViewportSize = glm::ivec2(1, 1);
			m_mouseInViewport = false;
			m_viewportFocused = false;
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void ViewportPanel::handleDragDrop() {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_ASSET, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoPreviewTooltip)) {
				size_t assetCount = payload->DataSize / sizeof(UUID);
				const UUID* assetUUIDs = (const UUID*)payload->Data;

				for (size_t i = 0; i < assetCount; i++) {
					UUID droppedUUID = assetUUIDs[i];
					AssetMetadata* metaData = AssetRegistry::getAssetMetadata(droppedUUID);
					if (!metaData) {
						throw std::runtime_error("Asset dropped but not found in registry");
					}

					if (payload->IsDelivery()) {
						if (metaData->getType() == AssetType::TEXTURE) {

						} else if (metaData->getType() == AssetType::MODEL) {

							auto createAsset = [metaData] {
								Ref<Model> asset = AssetRegistry::getAsset(metaData);
								auto& ecs = SceneManager::getActiveScene()->getECS();


								Entity baseEntity = ecs.createEntity(asset->getName());
								SceneGraphComponent* sponzaParent = ecs.getComponent<SceneGraphComponent>(baseEntity);

								for (auto& mesh : asset->getSubMeshes()) {
									Entity e = ecs.createEntity(mesh->getName());
									SceneGraphComponent* r = ecs.getComponent<SceneGraphComponent>(e);
									ecs.addComponent<MeshRendererComponent>(e, mesh);
									sponzaParent->addChild(r);
								}
							};

							if (AssetRegistry::isAssetStreamed(metaData)) {
								createAsset();
							} else {
								AssetRegistry::streamAsset(metaData);
								EventSystem::subscribeOnce<AssetStreamedEvent>([createAsset](AssetStreamedEvent&) {
									createAsset();
								});
							}
						} else if (metaData->getType() == AssetType::MATERIAL) {

						} else if (metaData->getType() == AssetType::SHADER) {

						}
					}
				}

				DragDrop::setIsAtValidDropLocation();
			}
			ImGui::EndDragDropTarget();
		}
	}
};