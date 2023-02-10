#include "pch.h"
#include "CppUnitTest.h"
#include "physics.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PMG
{
	TEST_CLASS(PhysicsTest) {
		public:
			TEST_METHOD(TestComp) {
				float a = 1.001f;
				float b = 1.001f;
				constexpr float c = 1.001f + std::numeric_limits<float>::epsilon();
				float d = 2.001f;

				Assert::IsTrue(comp(a, b), L"Equal floats not equal");
				Assert::IsTrue(comp(a, c), L"Floats with <= epsilon difference not equal");
				Assert::IsFalse(comp(a, d), L"Different floats not unequal");
			}
	};
}
