#include <iostream>
#include <cmath>

constexpr double PI = 3.14159265358979323846;
double degrees_to_radians(const double degrees) {
    return (PI / 180) * degrees;
}
constexpr double EPSILON = 1e-6;

constexpr double a = 6378137.0; // WGS 84 equatorial radius 
constexpr double f = 1.0 / 298.257223563; // WGS 84 flattening
constexpr double e2 = f * (2 - f); // Square of the first numerical eccentricity
const double N(const double phi) {  // Distance from the surface to the Z-axis along the ellipsoid normal
    const double sin_phi = sin(phi);
    return a / sqrt(1 - (e2 * (sin_phi * sin_phi)));
}

struct GeoditicCoord {
    double latitude;
    double longitude;
    double height;
};

struct EcefCoord {
    double x;
    double y;
    double z;
};

struct EnuCoord {
    double e;
    double n;
    double u;
};

void geodetic_to_ecef(const GeoditicCoord& geo, EcefCoord* ecef) {
    const double lambda = degrees_to_radians(geo.longitude);
    const double phi = degrees_to_radians(geo.latitude);
    const double h = geo.height;

    const double N_phi = N(phi);
    const double sin_phi = sin(phi);
    const double cos_phi = cos(phi);
    const double sin_lambda = sin(lambda);
    const double cos_lambda = cos(lambda);

    ecef->x = (N_phi + h) * cos_phi * cos_lambda;
    ecef->y = (N_phi + h) * cos_phi * sin_lambda;
    ecef->z = ((1 - e2) * N_phi + h) * sin_phi;
}

void test_assert(const bool val, const std::string& error_msg) {
    if (!val) {
        std::cerr << "Assert failed: " << error_msg;
        std::abort();
    }
}

bool almost_equal(const double a, const double b) {
    return abs(a - b) < EPSILON;
}

void test_geodetic_to_ecef() {
    GeoditicCoord geo{34.00000048, -117.3335693, 251.702};
    EcefCoord ecef{};
    geodetic_to_ecef(geo, &ecef);
    test_assert(almost_equal(ecef.x, -2430601.8), "ecef.x wrong");
    test_assert(almost_equal(ecef.y, -4702442.7), "ecef.y wrong");
    test_assert(almost_equal(ecef.z, 3546587.4), "ecef.z wrong");
}

int main(int argc, char* argv[]) {
    test_geodetic_to_ecef();

    return 0;
}
