#pragma once

namespace PMG::Physics {
	class Vector3 {
	public:
		Vector3();
		Vector3(double x, double y, double z);

		double x;
		double y;
		double z;
	};
}