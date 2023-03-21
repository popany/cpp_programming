#include "core/basic_types.h"
#include "core/bezier_curve.h"
#include <glog/logging.h>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

namespace crv {

class BezierCurve {
public:
  BezierCurve(const std::vector<Point2D> &control_points) {
    if (control_points.empty()) {
      return;
    }

    switch (control_points.size() - 1) {
#define BEZIER_CURVE_ORDER_CASE(order)                                         \
  case order:                                                                  \
    curve_ = std::make_unique<BezierCurveRecursiveDefine<order>>(              \
        &control_points[0]);                                                   \
    break;

      BEZIER_CURVE_ORDER_CASE(0)
      BEZIER_CURVE_ORDER_CASE(1)
      BEZIER_CURVE_ORDER_CASE(2)
      BEZIER_CURVE_ORDER_CASE(3)
      BEZIER_CURVE_ORDER_CASE(4)
      BEZIER_CURVE_ORDER_CASE(5)
      BEZIER_CURVE_ORDER_CASE(6)
      BEZIER_CURVE_ORDER_CASE(7)
      BEZIER_CURVE_ORDER_CASE(8)
      BEZIER_CURVE_ORDER_CASE(9)
      BEZIER_CURVE_ORDER_CASE(10)
#undef BEZIER_CURVE_ORDER_CASE
    }
  }

  static BezierCurve create(const std::vector<Point2D> control_points) {
    return BezierCurve(control_points);
  }

  Point2D calc(const double t) {
    if (curve_) {
      return curve_->calc(t);
    } else {
      return {0, 0};
    }
  }

private:
  std::unique_ptr<BezierCurveInterface> curve_;
};

} // namespace crv

namespace py = pybind11;

PYBIND11_MODULE(libcurve_py, m) {
  py::class_<crv::Point2D>(m, "Point2D")
      .def(py::init())
      .def(py::init<double, double>())
      .def_readwrite("x", &crv::Point2D::x)
      .def_readwrite("y", &crv::Point2D::y);

  py::class_<crv::BezierCurve>(m, "BezierCurve")
      .def("create", &crv::BezierCurve::create)
      .def("calc", &crv::BezierCurve::calc);
}
