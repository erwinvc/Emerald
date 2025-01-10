#pragma once

#include "utils/core/log.h"
#include "core/common/rtti.h"
namespace emerald::tests {
	class Test {
		RTTI_BASE_CLASS_DECL(Test)
	};

	class Test2 : public Test {
		RTTI_DERIVED_CLASS_DECL(Test2, Test)
	};

	class Test3 : public Test2 {
		RTTI_DERIVED_CLASS_DECL(Test3, Test2)
	};

	RTTI_CLASS_DEF(Test);
	RTTI_CLASS_DEF(Test2);
	RTTI_CLASS_DEF(Test3);

	bool testRTTI() {
		// Create instances of Test, Test2, and Test3
		Test base;
		Test2 derived;
		Test3 derived2;

		// Track the overall success of the tests
		bool allTestsPassed = true;

		// Check if the instances are recognized as their respective types
		bool baseIsTest = base.isClassType(Test::getClassType());
		bool derivedIsTest2 = derived.isClassType(Test2::getClassType());
		bool derivedIsTest = derived.isClassType(Test::getClassType());

		Log::info("Testing if base is of type Test: {}", baseIsTest);
		Log::info("Testing if derived is of type Test2: {}", derivedIsTest2);
		Log::info("Testing if derived is of type Test: {}", derivedIsTest);

		allTestsPassed &= baseIsTest;
		allTestsPassed &= derivedIsTest2;
		allTestsPassed &= derivedIsTest;

		// Additional checks for Test3
		bool derived2IsTest3 = derived2.isClassType(Test3::getClassType());
		bool derived2IsTest2 = derived2.isClassType(Test2::getClassType());
		bool derived2IsTest = derived2.isClassType(Test::getClassType());

		Log::info("Testing if derived2 is of type Test3: {}", derived2IsTest3);
		Log::info("Testing if derived2 is of type Test2: {}", derived2IsTest2);
		Log::info("Testing if derived2 is of type Test: {}", derived2IsTest);

		allTestsPassed &= derived2IsTest3;
		allTestsPassed &= derived2IsTest2;
		allTestsPassed &= derived2IsTest;

		// Checking RTTI mechanism for false positives
		bool isTestDerived = derived.isClassType(Test2::getClassType());
		bool isBaseDerived = base.isClassType(Test2::getClassType());

		Log::info("Derived instance recognized as Test2: {}", isTestDerived);
		Log::info("Base instance incorrectly recognized as Test2: {}", isBaseDerived);

		allTestsPassed &= isTestDerived;
		allTestsPassed &= !isBaseDerived; // Expect false, so negate to match success

		// Checking RTTI mechanism for Test3 from an instance of Test2
		bool isTest3Derived = derived2.isClassType(Test3::getClassType());
		Log::info("Derived2 instance recognized as Test3: {}", isTest3Derived);

		allTestsPassed &= isTest3Derived;

		// Check if the derived class has the correct base class types
		bool derivedCorrectBaseType = derived.getParentClassType() == Test::getClassType();
		bool derived2CorrectBaseType = derived2.getParentClassType() == Test2::getClassType();
		Log::info("Derived class has the correct base class type: {}", derivedCorrectBaseType);
		Log::info("Derived2 class has the correct base class type: {}", derived2CorrectBaseType);

		allTestsPassed &= derivedCorrectBaseType;
		allTestsPassed &= derived2CorrectBaseType;

		// Test the template checkRTTI function
		bool testRTTI = Test::checkRTTI<Test>();
		bool test2RTTI = Test2::checkRTTI<Test2>();
		bool test3RTTI = Test3::checkRTTI<Test3>();

		Log::info("Test RTTI check: {}", testRTTI);
		Log::info("Test2 RTTI check: {}", test2RTTI);
		Log::info("Test3 RTTI check: {}", test3RTTI);

		allTestsPassed &= testRTTI;
		allTestsPassed &= test2RTTI;

		return allTestsPassed;
	}
}