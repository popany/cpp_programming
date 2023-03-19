#pragma once

#include "core/basic_types.h"

namespace crv {

inline Point2D operator*(const double v, const Point2D &p) {
  return {p.x * v, p.y * v};
}

inline Point2D operator+(const Point2D &p_a, const Point2D &p_b) {
  return {p_a.x + p_b.x, p_a.y + p_b.y};
}

} // namespace crv
