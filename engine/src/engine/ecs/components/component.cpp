#include "eepch.h"
#include "component.h"
#include "../core/entityComponentSystem.h"
#include "sceneGraphComponent.h"
#include "../../scene/sceneManager.h"
#include "metadataComponent.h"
#include "../src/utils/text/stringUtils.h"

namespace emerald {
	RTTI_CLASS_DEF(Component);

	uint64_t Component::getHash() {
		EntityComponentSystem& ecs = SceneManager::getActiveScene()->getECS();
		std::string str = std::string(getComponentTypeInfo().name.c_str());

		SceneGraphComponent* comp = ecs.getComponent<SceneGraphComponent>(m_entity);

		std::vector<MetadataComponent*> components;
		while (comp != SceneManager::getActiveScene()->getRootNode()) {
			components.push_back(ecs.getComponent<MetadataComponent>(comp->m_entity));
			comp = comp->getParent();
		}

		for(auto* metadata : components) {
			str += std::format("->{}", metadata->getName().c_str());
		}

		return StringUtils::hash64(str);
	}
}