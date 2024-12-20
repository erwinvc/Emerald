#pragma once

#define DRAG_DROP_ASSET "_ASSET"

namespace emerald {
	class DragDrop {
	public:
		static void setIsAtValidDropLocation();
		static void handleDragDropVisuals();
	};
}