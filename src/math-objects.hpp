#pragma once

#include <vector>
#include <cstddef>

namespace Math {

    struct Point {
        double x;
        double y;
        double z;
    };

    struct curve {
        std::vector<Point> points;
    };

    struct frenet {
        Point tangent;
        Point normal;
        Point binormal;
    };

    Point normalize(const Point& p);

    double distance(const Point& p1, const Point& p2);

    double curvature(const curve& c, std::size_t index);

    double torsion(const curve& c, std::size_t index);

    frenet computeFrenetFrame(const curve& c, std::size_t index);

    Point helix(double a, double b, double t);
    Point circle(double centre, double radius, double t);
    Point torusKnot(double R, double r, double t, double p, double q);

    curve generateCircle(
        double radius,
        double start,
        double end,
        double step
    );

    curve generateTorusKnot(
        double R,
        double r,
        double p,
        double q,
        double start,
        double end,
        double step
    );

    curve generateHelix(
        double a,
        double b,
        double start,
        double end,
        double step
    );

    Point helixArcLength(double a, double b, double s);

    curve generateHelixArcLength(
        double a,
        double b,
        double start,
        double end,
        double step
    );
}