#include "CppUnitTest.h"
#include "pmg_physics.h"

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

				Assert::IsTrue(Physics::CompareFloat(a, b), L"Equal floats not equal");
				Assert::IsTrue(Physics::CompareFloat(a, c), L"Floats with <= epsilon difference not equal");
				Assert::IsFalse(Physics::CompareFloat(a, d), L"Different floats not unequal");
			}

			TEST_METHOD(TestSphereCollision) {
				// Ray to the right, pointing left
				Physics::Sphere sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				Physics::Ray ray = Physics::Ray(Physics::Vector3(3, 0, 0), Physics::Vector3(-1, 0, 0));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray to the right, pointing right
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(3, 0, 0), Physics::Vector3(1, 0, 0));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray above, pointing down
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 3, 0), Physics::Vector3(0, -1, 0));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray above, pointing up
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 3, 0), Physics::Vector3(0, 1, 0));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray left, pointing right
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(-3, 0, 0), Physics::Vector3(1, 0, 0));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray left, pointing left
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(-3, 0, 0), Physics::Vector3(-1, 0, 0));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray below, pointing up
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, -3, 0), Physics::Vector3(0, 1, 0));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray below, pointing down
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, -3, 0), Physics::Vector3(0, -1, 0));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray in front, pointing backward
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 0, -3), Physics::Vector3(0, 0, 1));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray in front, pointing forward
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 0, -3), Physics::Vector3(0, 0, -1));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray behind, pointing forward
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 0, 3), Physics::Vector3(0, 0, -1));

				Assert::IsTrue(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Ray behind, pointing backward
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 0, 3), Physics::Vector3(0, 0, 1));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");

				// Miss
				sphere = Physics::Sphere(Physics::Vector3(0, 0, 0), 1);
				ray = Physics::Ray(Physics::Vector3(0, 0, -3), Physics::Vector3(1, 0, 1));

				Assert::IsFalse(Physics::TestCollision(ray, sphere), L"Sphere-Ray collision test failed");
			}
	};
}
