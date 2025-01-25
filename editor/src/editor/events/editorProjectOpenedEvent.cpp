#include "eepch.h"
#include "editorProjectOpenedEvent.h"
#include "core/common/rtti.h"

namespace emerald {
	//struct {
	//	RTTIType a = EditorProjectOpenedEvent::getStaticParentClassType();
	//	RTTIType b = EditorProjectOpenedEvent::getStaticClassType();
	//	RTTIType c = Event::getStaticClassType();
	//	RTTIType d = typeid(int);
	//} test;

	RTTI_CLASS_DEF(EditorProjectOpenedEvent);
}