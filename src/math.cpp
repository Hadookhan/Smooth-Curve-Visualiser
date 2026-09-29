#include "math-objects.hpp"

#include <cmath>
#include <stdexcept>

namespace Math {

    double distance(const Point& p1, const Point& p2) {
        const double dx = p2.x - p1.x;
        const double dy = p2.y - p1.y;
        const double dz = p2.z - p1.z;

        return std::sqrt(
            dx * dx +
            dy * dy +
            dz * dz
        );
    }

    Point normalize(const Point& p) {
        const double length = std::sqrt(
            p.x * p.x +
            p.y * p.y +
            p.z * p.z
        );

        if (length < 1e-10) {
            throw std::runtime_error(
                "Cannot normalize a zero vector."
            );
        }

        return {
            p.x / length,
            p.y / length,
            p.z / length
        };
    }

    Point circle(double centre, double radius, double t) {
        return {
            centre + radius * std::cos(t/radius),
            centre + radius * std::sin(t/radius),
            0.0
        };
    }

    Point torusKnot(double R, double r, double t, double p, double q) {
        return {
            (R + r * std::cos(q * t)) * std::cos(p * t),
            (R + r * std::cos(q * t)) * std::sin(p * t),
            r * std::sin(q * t)
        };
    }

    Point helix(double a, double b, double t) {

        const double denominator =
            std::sqrt(a * a + b * b);

        if (denominator < 1e-10) {
            throw std::invalid_argument(
                "a and b cannot both be zero."
            );
        }

        return {
            a * std::cos(t / denominator),
            a * std::sin(t / denominator),
            (b * t) / denominator
        };
    }

    curve generateCircle(
        double radius,
        double start,
        double end,
        double step
    ) {
        if (step <= 0.0) {
            throw std::invalid_argument(
                "Circle step must be positive."
            );
        }

        curve c;

        for (double t = start; t <= end; t += step) {
            c.points.push_back(
                circle(0.0, radius, t)
            );
        }

        return c;
    }

    curve generateTorusKnot(
        double R,
        double r,
        double p,
        double q,
        double start,
        double end,
        double step
    ) {
        if (step <= 0.0) {
            throw std::invalid_argument(
                "Torus knot step must be positive."
            );
        }

        curve c;

        for (double t = start; t <= end; t += step) {
            c.points.push_back(
                torusKnot(R, r, t, p, q)
            );
        }

        return c;
    }

    curve generateHelix(
        double a,
        double b,
        double start,
        double end,
        double step
    ) {
        if (step <= 0.0) {
            throw std::invalid_argument(
                "Helix step must be positive."
            );
        }

        curve c;

        for (double t = start; t <= end; t += step) {
            c.points.push_back(
                helix(a, b, t)
            );
        }

        return c;
    }

    double curvature(const curve& c, std::size_t index) {

        if (index + 2 >= c.points.size()) {
            throw std::out_of_range(
                "Not enough points to compute curvature."
            );
        }

        const Point& p0 = c.points[index];
        const Point& p1 = c.points[index + 1];
        const Point& p2 = c.points[index + 2];

        Point v0 {
            p1.x - p0.x,
            p1.y - p0.y,
            p1.z - p0.z
        };

        Point v1 {
            p2.x - p1.x,
            p2.y - p1.y,
            p2.z - p1.z
        };

        Point T0 = normalize(v0);
        Point T1 = normalize(v1);

        Point dT {
            T1.x - T0.x,
            T1.y - T0.y,
            T1.z - T0.z
        };

        // Approximation of Δs
        double ds = distance(p0, p1);

        if (ds < 1e-10) {
            throw std::runtime_error(
                "Cannot compute curvature with zero arc length."
            );
        }

        double dTLength = std::sqrt(
            dT.x * dT.x +
            dT.y * dT.y +
            dT.z * dT.z
        );

        return dTLength / ds;
    }

    double torsion(const curve& c, std::size_t index) {

        if (index + 3 >= c.points.size()) {
            throw std::out_of_range(
                "Not enough points to compute torsion."
            );
        }

        frenet frame0 =
            computeFrenetFrame(c, index);

        frenet frame1 =
            computeFrenetFrame(c, index + 1);

        // Approximate dB
        Point dB {
            frame1.binormal.x - frame0.binormal.x,
            frame1.binormal.y - frame0.binormal.y,
            frame1.binormal.z - frame0.binormal.z
        };

        double ds =
            distance(
                c.points[index],
                c.points[index + 1]
            );

        if (ds < 1e-10) {
            throw std::runtime_error(
                "Cannot compute torsion with zero arc length."
            );
        }

        // dB/ds
        dB.x /= ds;
        dB.y /= ds;
        dB.z /= ds;

        // tau = -(dB/ds · N)

        double tau = -(
            dB.x * frame0.normal.x +
            dB.y * frame0.normal.y +
            dB.z * frame0.normal.z
        );

        return tau;
    }

    frenet computeFrenetFrame(
        const curve& c,
        std::size_t index
    ) {
        // We need p0, p1 and p2.
        if (index + 2 >= c.points.size()) {
            throw std::out_of_range(
                "Not enough points to compute Frenet frame."
            );
        }

        const Point& p0 = c.points[index];
        const Point& p1 = c.points[index + 1];
        const Point& p2 = c.points[index + 2];

        // Approximate tangent at p0.
        Point v0 {
            p1.x - p0.x,
            p1.y - p0.y,
            p1.z - p0.z
        };

        // Approximate tangent at p1.
        Point v1 {
            p2.x - p1.x,
            p2.y - p1.y,
            p2.z - p1.z
        };

        Point T0 = normalize(v0);
        Point T1 = normalize(v1);

        // Approximate dT/ds.
        Point dT {
            T1.x - T0.x,
            T1.y - T0.y,
            T1.z - T0.z
        };

        // N = (dT/ds) / ||dT/ds||
        Point N = normalize(dT);

        // B = T x N
        Point B {
            T0.y * N.z - T0.z * N.y,
            T0.z * N.x - T0.x * N.z,
            T0.x * N.y - T0.y * N.x
        };

        B = normalize(B);

        return {
            T0,
            N,
            B
        };
    }
}