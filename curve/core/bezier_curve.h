#pragma once

#include "core/basic_types.h"
#include "core/utils.h"
#include <stdint.h>

namespace crv {

class BezierCurveInterface {
public:
  virtual Point2D calc(const double t) = 0;
};

template <uint32_t order>
class BezierCurveRecursiveDefine : public BezierCurveInterface {
public:
  BezierCurveRecursiveDefine(const Point2D *p) : c_a_{p}, c_b_{p + 1} {}
  Point2D calc(const double t) override {
    return (1 - t) * c_a_.calc(t) + t * c_b_.calc(t);
  }

private:
  BezierCurveRecursiveDefine<order - 1> c_a_;
  BezierCurveRecursiveDefine<order - 1> c_b_;
};

template <> class BezierCurveRecursiveDefine<0> : public BezierCurveInterface {
public:
  BezierCurveRecursiveDefine(const Point2D *p) : p_{*p} {}
  Point2D calc(const double t) override { return p_; }

private:
  Point2D p_;
};

} // namespace crv
