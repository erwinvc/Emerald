#include "eepch.h"
#include "test.h"
#include "utils/uuid/uuidGenerator.h"
#include <unordered_set>
#include "utils/system/timer.h"

namespace emerald::tests {
	class TestClass : public RefCounted {
	public:
		int value;
		TestClass(int v) : value(v) {}
		bool operator==(const TestClass& other) const {
			return value == other.value;
		}
	};

	class TestObject : public RefCounted {
	public:
		TestObject(const std::string& name) : m_name(name) {
			Log::info("TestObject '{}' constructed", m_name);
		}

		~TestObject() {
			Log::info("TestObject '{}' destroyed", m_name);
		}

		std::string getName() const { return m_name; }

	private:
		std::string m_name;
	};

	class DerivedTestObject : public TestObject {
	public:
		DerivedTestObject(const std::string& name) : TestObject(name) {
			Log::info("DerivedTestObject '{}' constructed", getName());
		}

		~DerivedTestObject() {
			Log::info("DerivedTestObject '{}' destroyed", getName());
		}
	};

	class RefCountingTests {
	public:
		static void RunAllTests() {
			Log::info("Starting Reference Counting Tests");

			TestBasicRefCreation();
			TestRefAssignment();
			TestWeakRefBasics();
			TestCircularReferences();
			TestInheritance();
			TestMoveSemantics();
			TestNullHandling();
			TestWeakRefExpiration();
			TestMultipleWeakRefs();
			TestMixedReferences();
			TestThreadSafety();
			TestContainerUsage();
			TestResetAndRelease();

			Log::info("All Reference Counting Tests Completed Successfully");
		}

	private:
		static void TestBasicRefCreation() {
			Log::info("Testing basic Ref creation and destruction");

			{
				Ref<TestObject> ref = Ref<TestObject>::create("BasicTest");
				assert(ref->getName() == "BasicTest");
				assert(ref.getRefCount() == 1);
				Log::info("Reference count: {}", ref.getRefCount());
			}
			// ref should be destroyed here

			{
				// Test create with multiple arguments
				auto ref = Ref<TestObject>::create("MultiArgTest");
				assert(ref.getRefCount() == 1);
				Log::info("Created object with name: {}", ref->getName());
			}
		}

		static void TestRefAssignment() {
			Log::info("Testing Ref assignment operations");

			Ref<TestObject> ref1 = Ref<TestObject>::create("Assignment1");
			Ref<TestObject> ref2 = Ref<TestObject>::create("Assignment2");

			assert(ref1.getRefCount() == 1);
			assert(ref2.getRefCount() == 1);

			// Test copy assignment
			Ref<TestObject> ref3 = ref1;
			assert(ref1.getRefCount() == 2);
			Log::info("After copy assignment, ref count: {}", ref1.getRefCount());

			// Test self-assignment
			ref1 = ref1;
			assert(ref1.getRefCount() == 2);

			// Test nullptr assignment
			ref3 = nullptr;
			assert(ref1.getRefCount() == 1);
			assert(!ref3);

			Log::info("Assignment tests completed");
		}

		static void TestWeakRefBasics() {
			Log::info("Testing WeakRef basic operations");

			WeakRef<TestObject> weakRef;
			{
				Ref<TestObject> strongRef = Ref<TestObject>::create("WeakTest");
				weakRef = strongRef;

				assert(!weakRef.expired());
				assert(weakRef.lock());

				Ref<TestObject> strongRef2 = weakRef.lock();
				assert(strongRef2.getRefCount() == 2);
				Log::info("Weak ref successfully locked, ref count: {}", strongRef2.getRefCount());
			}

			assert(weakRef.expired());
			assert(!weakRef.lock());
			Log::info("Weak ref correctly expired after strong ref destruction");
		}

		static void TestCircularReferences() {
			Log::info("Testing circular reference handling");

			struct Node : public RefCounted {
				Node(const std::string& name) : name(name) {}
				std::string name;
				WeakRef<Node> next;  // Using WeakRef to prevent cycles
			};

			Ref<Node> node1 = Ref<Node>::create("Node1");
			Ref<Node> node2 = Ref<Node>::create("Node2");

			node1->next = node2;
			node2->next = node1;

			Log::info("Created circular reference chain");

			// Let references go out of scope
			node1 = nullptr;
			node2 = nullptr;

			Log::info("Circular references cleaned up");
		}

		static void TestInheritance() {
			Log::info("Testing inheritance relationships");

			Ref<DerivedTestObject> derived = Ref<DerivedTestObject>::create("Derived");
			Ref<TestObject> base = derived;

			assert(derived.getRefCount() == 2);
			Log::info("Base and derived refs share count: {}", derived.getRefCount());

			// Test dynamic casting
			Ref<DerivedTestObject> derivedAgain = base.as<DerivedTestObject>();
			assert(derivedAgain);
			assert(derived.getRefCount() == 3);

			// Test failed cast
			Ref<TestObject> pureBase = Ref<TestObject>::create("PureBase");
			Ref<DerivedTestObject> shouldBeNull = pureBase.as<DerivedTestObject>();
			assert(!shouldBeNull);

			Log::info("Inheritance tests completed");
		}

		static void TestMoveSemantics() {
			Log::info("Testing move semantics");

			Ref<TestObject> original = Ref<TestObject>::create("MoveTest");
			Ref<TestObject> moved = std::move(original);

			assert(!original);
			assert(moved);
			assert(moved.getRefCount() == 1);

			// Test move assignment
			Ref<TestObject> target = Ref<TestObject>::create("Target");
			target = std::move(moved);

			assert(!moved);
			assert(target);
			assert(target.getRefCount() == 1);

			Log::info("Move semantics tests completed");
		}

		static void TestNullHandling() {
			Log::info("Testing null handling");

			Ref<TestObject> nullRef;
			assert(!nullRef);

			WeakRef<TestObject> nullWeak;
			assert(nullWeak.expired());

			// Test null assignment
			Ref<TestObject> ref = Ref<TestObject>::create("NullTest");
			ref = nullptr;
			assert(!ref);

			// Test comparison with nullptr
			assert(ref == nullptr);
			assert(nullptr == ref);
			assert(!(ref != nullptr));

			Log::info("Null handling tests completed");
		}

		static void TestWeakRefExpiration() {
			Log::info("Testing weak reference expiration scenarios");

			WeakRef<TestObject> weak;
			{
				Ref<TestObject> strong = Ref<TestObject>::create("ExpirationTest");
				weak = strong;

				assert(!weak.expired());
				{
					Ref<TestObject> locked = weak.lock();
					assert(locked);
					assert(locked.getRefCount() == 2);
				}
			}

			assert(weak.expired());
			assert(!weak.lock());

			Log::info("Weak reference expiration tests completed");
		}

		static void TestMultipleWeakRefs() {
			Log::info("Testing multiple weak references");

			std::vector<WeakRef<TestObject>> weakRefs;
			{
				Ref<TestObject> strong = Ref<TestObject>::create("MultiWeakTest");

				// Create multiple weak references
				for (int i = 0; i < 5; ++i) {
					weakRefs.push_back(WeakRef<TestObject>(strong));
				}

				assert(strong.getRefCount() == 1);

				// Test all weak refs are valid
				for (const auto& weak : weakRefs) {
					assert(!weak.expired());
					assert(weak.lock());
				}

				Log::info("Created {} weak references to the same object", weakRefs.size());
			}

			// Test all weak refs are expired
			for (const auto& weak : weakRefs) {
				assert(weak.expired());
				assert(!weak.lock());
			}

			Log::info("Multiple weak references test completed");
		}

		static void TestMixedReferences() {
			Log::info("Testing mixed strong and weak reference scenarios");

			Ref<TestObject> strong1 = Ref<TestObject>::create("MixedTest");
			WeakRef<TestObject> weak1(strong1);
			WeakRef<TestObject> weak2(strong1);

			{
				Ref<TestObject> strong2 = weak1.lock();
				assert(strong2);
				assert(strong1.getRefCount() == 2);

				// Test weak ref while multiple strong refs exist
				WeakRef<TestObject> weak3(strong2);
				assert(!weak3.expired());
			}

			assert(strong1.getRefCount() == 1);
			assert(!weak1.expired());
			assert(!weak2.expired());

			Log::info("Mixed references test completed");
		}

		static void TestThreadSafety() {
			Log::info("Testing thread safety of reference counting");

			Ref<TestObject> shared = Ref<TestObject>::create("ThreadTest");
			WeakRef<TestObject> weak(shared);

			std::vector<std::thread> threads;
			const int numThreads = 10;
			const int operationsPerThread = 1000;

			for (int i = 0; i < numThreads; ++i) {
				threads.emplace_back([shared, &weak, operationsPerThread]() {
					for (int j = 0; j < operationsPerThread; ++j) {
						// Mix of operations that could race
						Ref<TestObject> local = shared;
						WeakRef<TestObject> localWeak = weak;
						Ref<TestObject> fromWeak = weak.lock();

						// Let the references go out of scope
					}
				});
			}

			for (auto& thread : threads) {
				thread.join();
			}

			assert(shared.getRefCount() == 1);
			assert(!weak.expired());

			Log::info("Thread safety test completed");
		}

		static void TestContainerUsage() {
			Log::info("Testing container usage with Ref and WeakRef");

			std::vector<Ref<TestObject>> strongRefs;
			std::vector<WeakRef<TestObject>> weakRefs;

			// Add elements
			for (int i = 0; i < 5; ++i) {
				strongRefs.push_back(Ref<TestObject>::create(std::format("Container{}", i)));
				weakRefs.push_back(WeakRef<TestObject>(strongRefs.back()));
			}

			// Test container operations
			strongRefs.erase(strongRefs.begin());
			assert(weakRefs[0].expired());

			// Clear strong references
			strongRefs.clear();

			// Verify all weak references are expired
			for (const auto& weak : weakRefs) {
				assert(weak.expired());
			}

			Log::info("Container usage test completed");
		}

		static void TestResetAndRelease() {
			Log::info("Testing reset and release operations");

			// Test reset
			{
				Ref<TestObject> ref = Ref<TestObject>::create("ResetTest");
				WeakRef<TestObject> weak(ref);

				ref.reset();  // This will delete the object
				assert(!ref);
				assert(weak.expired());
			}

			// Test release
			{
				Ref<TestObject> ref = Ref<TestObject>::create("ReleaseTest");
				WeakRef<TestObject> weak(ref);

				TestObject* rawPtr = ref.release();  // This transfers ownership
				assert(!ref);
				assert(weak.expired());

				// Now we can safely delete since we have ownership
				delete rawPtr;
			}

			Log::info("Reset and release test completed");
		}
	};

	void refCountingTests() {
		RefCountingTests a;
		a.RunAllTests();
		return;
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

	static std::vector<unsigned char> uuidToBytes(const std::string& uuid) {
		std::string hex;
		for (auto c : uuid) {
			if (c != '-') {
				hex.push_back(c);
			}
		}

		std::vector<unsigned char> bytes;
		bytes.reserve(16);
		for (size_t i = 0; i < hex.size(); i += 2) {
			std::string byteStr = hex.substr(i, 2);
			unsigned char byteVal = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
			bytes.push_back(byteVal);
		}
		return bytes;
	}

	void test() {

		Timer timer;
		for (int i = 0; i < 1000000; i++) {
			UUIDGenerator::createFast();
		}
		timer.print("fast");

		Timer timer2;
		for (int i = 0; i < 1000000; i++) {
			UUIDGenerator::createVersion1();
		}
		timer2.print("V1");

		Timer timer3;
		for (int i = 0; i < 1000000; i++) {
			UUIDGenerator::create();
		}
		timer3.print("V4");
			const size_t NUM_UUIDS = 100000; // Generate 100k UUIDs for a basic test
		Log::info("Generating {} UUIDs...", NUM_UUIDS);

		std::unordered_set<std::string> seen;
		seen.reserve(NUM_UUIDS);
		seen.max_load_factor(0.7);

		std::array<size_t, 128> bitCounts{};
		for (auto& count : bitCounts) {
			count = 0;
		}

		size_t duplicates = 0;
		for (size_t i = 0; i < NUM_UUIDS; ++i) {
			std::string uuid = UUIDGenerator::create();

			// Check for duplicates
			if (!seen.insert(uuid).second) {
				duplicates++;
			}

			// Convert to bytes and update bit counts
			auto bytes = uuidToBytes(uuid);
			for (size_t bitPos = 0; bitPos < 128; ++bitPos) {
				size_t byteIndex = bitPos / 8;
				size_t bitIndex = bitPos % 8;
				if ((bytes[byteIndex] >> bitIndex) & 0x01) {
					bitCounts[bitPos]++;
				}
			}
		}

		// Report duplicates
		Log::info("Duplicates found: {}/{} ({:.2f}%)", duplicates, NUM_UUIDS, (100.0 * duplicates / NUM_UUIDS));

		// Analyze bit distribution
		Log::info(""); // blank line
		Log::info("Bit distribution:");
		for (size_t i = 0; i < 128; ++i) {
			double ratio = static_cast<double>(bitCounts[i]) / NUM_UUIDS;
			// Using formatting to show bit index aligned and ratio with two decimal places
			Log::info("Bit {:3}: {:.2f}% set", i, (ratio * 100.0));
		}

		double sum = 0.0;
		for (auto c : bitCounts) {
			double r = static_cast<double>(c) / NUM_UUIDS;
			sum += r;
		}
		double mean = sum / 128.0;

		double sqSum = 0.0;
		for (auto c : bitCounts) {
			double r = static_cast<double>(c) / NUM_UUIDS;
			double diff = r - mean;
			sqSum += diff * diff;
		}
		double variance = sqSum / 128.0;
		double stddev = std::sqrt(variance);

		Log::info("");
		Log::info("Average bit set ratio: {:.2f}%", (mean * 100.0));
		Log::info("Standard deviation of bit set ratio: {:.2f}%", (stddev * 100.0));
	}
}