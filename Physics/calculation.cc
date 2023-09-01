#include "calculation.h"
#include <math.h>
#include <limits>
#include <cmath>

namespace PMG::Physics {
	bool CompareDouble(double a, double b) {
		return abs(a - b) <= std::numeric_limits<double>::epsilon();
	}
}