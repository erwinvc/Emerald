#include "eepch.h"
#include "component.h"
#include "../core/entityComponentSystem.h"
#include "sceneGraphComponent.h"
#include "../../scene/sceneManager.h"
#include "metadataComponent.h"
#include "../src/utils/text/stringUtils.h"
#include "../core/ECSManager.h"

namespace emerald {
	RTTI_CLASS_DEF(Component);

	uint64_t Component::getHash() {
		std::string str = std::string(getComponentTypeInfo().name.c_str());

		SceneGraphComponent* comp = ECSManager::ECS().getComponent<SceneGraphComponent>(m_entity);

		std::vector<MetadataComponent*> components;
		while (comp != SceneManager::getActiveScene()->getRootNode()) {
			components.push_back(ECSManager::ECS().getComponent<MetadataComponent>(comp->m_entity));
			comp = comp->getParent();
		}

		for(auto* metadata : components) {
			str += std::format("->{}", metadata->getName().c_str());
		}

		return StringUtils::hash64(str);
	}
}