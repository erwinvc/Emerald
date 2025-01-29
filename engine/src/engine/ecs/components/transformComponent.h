#pragma once
#include "component.h"
#include "engine/ecs/core/entity.h"
#include "componentRegistry.h"

namespace emerald {
	class TransformComponent : public Component {
		COMPONENT_DECL(TransformComponent)
	public:
		void setPosition(const glm::vec3& position) { m_position = position; markDirty(); }
		void setRotation(const glm::vec3& rotation) { m_rotation = rotation; markDirty(); }
		void setScale(const glm::vec3& scale) { m_scale = scale; markDirty(); }
		const glm::vec3& getPosition() const { return m_position; }
		const glm::vec3& getRotation() const { return m_rotation; }
		const glm::vec3& getScale() const { return m_scale; }

		void updateLocalTransform();
		void updateGlobalTransform(const glm::mat4& parentTransform);
		const glm::mat4 getLocalTransform() const { return m_localTransform; }
		const glm::mat4 getGlobalTransform() const { return m_globalTransform; }
		
		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

		virtual nlohmann::json serialize() override;
		virtual void deserialize(const nlohmann::json& j) override;
		//static TransformComponent* deserialize(const nlohmann::json& json, Entity entity);

	private:
		friend class TransformComponentInspector;
		
		static inline ComponentTypeInfo s_componentTypeInfo = { "Transform", ComponentCategory::INFO, false };

		bool m_needsUpdate = true;
		glm::mat4 m_localTransform = glm::mat4(1.0f);
		glm::mat4 m_globalTransform = glm::mat4(1.0f);
		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

		void markDirty() { m_needsUpdate = true; }
	};
}