#include "eepch.h"
#include "test.h"

namespace emerald::tests {
	class TestClass : public RefCounted {
	public:
		int value;
		TestClass(int v) : value(v) {}
		bool operator==(const TestClass& other) const {
			return value == other.value;
		}
	};

	void test() {
		Log::info("Starting Ref class tests...");

		// Test 1: Default construction
		Ref<TestClass> ref1;
		Log::info("Test 1 - Default construction: {}", ref1.raw() == nullptr);
		assert(ref1.raw() == nullptr);

		// Test 2: Pointer construction
		auto* obj1 = new TestClass(10);
		Ref<TestClass> ref2(obj1);
		Log::info("Test 2 - Pointer construction: {}", ref2->value == 10 && ref2.getRefCount() == 1);
		assert(ref2->value == 10);
		assert(ref2.getRefCount() == 1);

		// Test 3: Copy construction
		Ref<TestClass> ref3(ref2);
		Log::info("Test 3 - Copy construction: {}", ref3->value == 10 && ref3.getRefCount() == 2);
		assert(ref3->value == 10);
		assert(ref3.getRefCount() == 2);

		// Test 4: Move construction
		Ref<TestClass> ref4(std::move(ref3));
		Log::info("Test 4 - Move construction: {}", ref4->value == 10 && ref4.getRefCount() == 2 && ref3.raw() == nullptr);
		assert(ref4->value == 10);
		assert(ref4.getRefCount() == 2);
		assert(ref3.raw() == nullptr);

		// Test 5: Copy assignment
		Ref<TestClass> ref5;
		ref5 = ref4;
		Log::info("Test 5 - Copy assignment: {}", ref5->value == 10 && ref5.getRefCount() == 3);
		assert(ref5->value == 10);
		assert(ref5.getRefCount() == 3);

		// Test 6: Move assignment
		Ref<TestClass> ref6;
		ref6 = std::move(ref5);
		Log::info("Test 6 - Move assignment: {}", ref6->value == 10 && ref6.getRefCount() == 3 && ref5.raw() == nullptr);
		assert(ref6->value == 10);
		assert(ref6.getRefCount() == 3);
		assert(ref5.raw() == nullptr);

		// Test 7: Reset
		ref6.reset();
		Log::info("Test 7 - Reset: {}", ref6.raw() == nullptr && ref4.getRefCount() == 2);
		assert(ref6.raw() == nullptr);
		assert(ref4.getRefCount() == 2);

		// Test 8: Equality operators
		Ref<TestClass> ref7(ref4);
		Log::info("Test 8 - Equality operators: {}", ref7 == ref4 && ref7 != ref2);
		assert(ref7 == ref4);

		// Test 9: Automatic deletion
		ref2.reset();
		ref4.reset();
		Log::info("Test 9 - Automatic deletion: {}", ref7.getRefCount() == 1);
		assert(ref7.getRefCount() == 1);

		ref7.reset();
		Log::info("Test 9 - Automatic deletion (after reset): {}", ref7.raw() == nullptr && ref7.getRefCount() == 0);
		assert(ref7.raw() == nullptr);
		assert(ref7.getRefCount() == 0);

		Log::info("All tests passed successfully.");
	}
}