#include "calculation.h"
#include <math.h>
#include <limits>
#include <cmath>

namespace PMG::Physics {
	bool CompareFloat(float a, float b) {
		return abs(a - b) <= std::numeric_limits<float>::epsilon();
	}
}