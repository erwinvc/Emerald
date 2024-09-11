#pragma once
#include "uuid.h"

namespace emerald {
	// Generator for UUIDs that aren't exactly universally unique, but are unique enough for our purposes.
	class UUIDGenerator {
	public:
		// Creates an engine specific UUID
		static UUID createFast();

		// Creates a version 1 UUID (time-based UUID)
		static UUID create();

	private:
		static void initialize();
	};
}