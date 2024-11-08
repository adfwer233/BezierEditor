#pragma once

#include <vector>
#include "geometry/curve/bezier_curve_2d.hpp"

struct Scene2D {
    std::vector<BezierCurve2D> curves;
};