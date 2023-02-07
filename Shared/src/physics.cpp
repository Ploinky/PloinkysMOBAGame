#include "physics.h"

namespace PMG {
  bool comp(float a, float b) {
    return abs(a - b) <= std::numeric_limits<float>::epsilon();
  }
}