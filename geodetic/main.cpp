// reference:
// https://en.wikipedia.org/wiki/Geographic_coordinate_conversion
// https://gist.github.com/govert/1b373696c9a27ff4c72a


#include <iostream>
#include <iomanip>
#include <cmath>

constexpr double PI = 3.14159265358979323846;
double degree_to_radian(const double degrees) {
    return (PI / 180) * degrees;
}
constexpr double EPSILON = 1e-6;
constexpr double centimeter = 0.01;

constexpr double a = 6378137.0;  // WGS 84 equatorial radius 
constexpr double f = 1.0 / 298.257223563;  // WGS 84 flattening
constexpr double e2 = f * (2 - f);  // Square of the first numerical eccentricity
const double N(const double phi) {  // Distance from the surface to the Z-axis along the ellipsoid normal
    const double sin_phi = std::sin(phi);
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

void geodetic_to_ecef(const double sin_phi, const double cos_phi, 
                      const double sin_lambda, const double cos_lambda,
                      const double N_phi, const double h,
                      EcefCoord* ecef) {
    ecef->x = (N_phi + h) * cos_phi * cos_lambda;
    ecef->y = (N_phi + h) * cos_phi * sin_lambda;
    ecef->z = ((1 - e2) * N_phi + h) * sin_phi;
}

void geodetic_to_ecef(const GeoditicCoord& geo, EcefCoord* ecef) {
    const double lambda = degree_to_radian(geo.longitude);
    const double phi = degree_to_radian(geo.latitude);
    const double h = geo.height;

    const double N_phi = N(phi);
    const double sin_phi = std::sin(phi);
    const double cos_phi = std::cos(phi);
    const double sin_lambda = std::sin(lambda);
    const double cos_lambda = std::cos(lambda);

    geodetic_to_ecef(sin_phi, cos_phi, sin_lambda, cos_lambda, N_phi, h, ecef);
}

void ecef_to_enu(const double sin_phi0, const double cos_phi0,
                 const double sin_lambda0, const double cos_lambda0,
                 const EcefCoord& ecef0, const EcefCoord& ecef,
                 EnuCoord* enu) {
    const double dx = ecef.x - ecef0.x;
    const double dy = ecef.y - ecef0.y;
    const double dz = ecef.z - ecef0.z;

    enu->e = -sin_lambda0 * dx + cos_lambda0 * dy;
    enu->n = -sin_phi0 * cos_lambda0 * dx +
             -sin_phi0 * sin_lambda0 * dy +
             cos_phi0 * dz;
    enu->u = cos_phi0 * cos_lambda0 * dx +
             cos_phi0 * sin_lambda0 * dy +
             sin_phi0 * dz;
}

void geodetic_to_enu(const GeoditicCoord& geo0, const GeoditicCoord& geo, EnuCoord* enu) {
    const double lambda0 = degree_to_radian(geo0.longitude);
    const double phi0 = degree_to_radian(geo0.latitude);
    const double h0 = geo0.height;

    const double N_phi0 = N(phi0);
    const double sin_phi0 = std::sin(phi0);
    const double cos_phi0 = std::cos(phi0);
    const double sin_lambda0 = std::sin(lambda0);
    const double cos_lambda0 = std::cos(lambda0);

    EcefCoord ecef0{};
    geodetic_to_ecef(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, N_phi0, h0, &ecef0);
    EcefCoord ecef{};
    geodetic_to_ecef(geo, &ecef);

    ecef_to_enu(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, ecef, enu);
}

void enu_to_ecef(const double sin_phi0, const double cos_phi0,
                 const double sin_lambda0, const double cos_lambda0,
                 const EcefCoord& ecef0, const EnuCoord& enu,
                 EcefCoord* ecef) {
    ecef->x = -sin_lambda0 * enu.e +
              -sin_phi0 * cos_lambda0 * enu.n +
              cos_phi0 * cos_lambda0 * enu.u +
              ecef0.x;
    ecef->y = cos_lambda0 * enu.e +
              -sin_phi0 * sin_lambda0 * enu.n +
              cos_phi0 * sin_lambda0 * enu.u +
              ecef0.y;
    ecef->z = cos_phi0 * enu.n +
              sin_phi0 * enu.u + ecef0.z;
}

void test_assert(const bool val, const std::string& error_msg) {
    if (!val) {
        std::cerr << "Assert failed: " << error_msg << std::endl;
        std::abort();
    }
}

bool almost_equal(const double a, const double b, const double epsilon) {
    const bool al_eq = std::abs(a - b) < epsilon;
    if (!al_eq) {
        std::cerr << std::fixed << std::setprecision(7); 
        std::cerr << a << " != " << b << std::endl;
    }
    return al_eq;
}

void test_geodetic_to_ecef() {  // https://www.convertecef.com
    GeoditicCoord geo{34.0000005, -117.3335693, 251.72};
    EcefCoord ecef{};
    geodetic_to_ecef(geo, &ecef);

    std::cout << std::fixed << std::setprecision(7); 
    std::cout << "> test_geodetic_to_ecef" << std::endl;
    std::cout << "ecef.x: " << ecef.x << std::endl;
    std::cout << "ecef.y: " << ecef.y << std::endl;
    std::cout << "ecef.z: " << ecef.z << std::endl;

    test_assert(almost_equal(ecef.x, -2430601.83, centimeter), "ecef.x wrong");
    test_assert(almost_equal(ecef.y, -4702442.72, centimeter), "ecef.y wrong");
    test_assert(almost_equal(ecef.z, 3546587.37, centimeter), "ecef.z wrong");
}

void test_ecef_to_enu() {
    std::cout << "> test_ecef_to_enu" << std::endl; 

    GeoditicCoord geo0{34.0000005, -117.3335693, 251.72};
    EcefCoord ecef0{};
    geodetic_to_ecef(geo0, &ecef0);

    const double sin_phi0 = std::sin(degree_to_radian(geo0.latitude));
    const double cos_phi0 = std::cos(degree_to_radian(geo0.latitude));
    const double sin_lambda0 = std::sin(degree_to_radian(geo0.longitude));
    const double cos_lambda0 = std::cos(degree_to_radian(geo0.longitude));

    EnuCoord enu{};
    ecef_to_enu(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, {ecef0.x + 1, ecef0.y, ecef0.z}, &enu);
    test_assert(almost_equal(enu.e, 0.88834836, EPSILON), "enu.e wrong");
    test_assert(almost_equal(enu.n, 0.25676467, EPSILON), "enu.n wrong");
    test_assert(almost_equal(enu.u, -0.38066927, EPSILON), "enu.u wrong");

    ecef_to_enu(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, {ecef0.x, ecef0.y + 1, ecef0.z}, &enu);
    test_assert(almost_equal(enu.e, -0.45917011, EPSILON), "enu.e wrong");
    test_assert(almost_equal(enu.n, 0.49675810, EPSILON), "enu.n wrong");
    test_assert(almost_equal(enu.u, -0.73647416, EPSILON), "enu.u wrong");

    ecef_to_enu(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, {ecef0.x, ecef0.y, ecef0.z + 1}, &enu);
    test_assert(almost_equal(enu.e, 0.0, EPSILON), "enu.e wrong");
    test_assert(almost_equal(enu.n, 0.82903757, EPSILON), "enu.n wrong");
    test_assert(almost_equal(enu.u, 0.55919291, EPSILON), "enu.u wrong");
}

void test_geodetic_to_enu() {
    GeoditicCoord geo0{46.017, 7.750, 1673};
    GeoditicCoord geo{45.976, 7.658, 4531};
    EnuCoord enu{};
    geodetic_to_enu(geo0, geo, &enu);

    std::cout << std::fixed << std::setprecision(7); 
    std::cout << "> test_geodetic_to_enu" << std::endl;
    std::cout << "enu.e: " << enu.e << std::endl;
    std::cout << "enu.n: " << enu.n << std::endl;
    std::cout << "enu.u: " << enu.u << std::endl;

    test_assert(almost_equal(enu.e, -7134.76, centimeter), "enu.e wrong");
    test_assert(almost_equal(enu.n, -4556.32, centimeter), "enu.n wrong");
    test_assert(almost_equal(enu.u, 2852.39, centimeter), "enu.u wrong");
}

void test_enu_to_ecef() {
    std::cout << "> test_ecef_to_enu" << std::endl; 

    GeoditicCoord geo0{34.0000005, -117.3335693, 251.72};
    EcefCoord ecef0{};
    geodetic_to_ecef(geo0, &ecef0);

    const double sin_phi0 = std::sin(degree_to_radian(geo0.latitude));
    const double cos_phi0 = std::cos(degree_to_radian(geo0.latitude));
    const double sin_lambda0 = std::sin(degree_to_radian(geo0.longitude));
    const double cos_lambda0 = std::cos(degree_to_radian(geo0.longitude));

    EnuCoord enu{};
    ecef_to_enu(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, {ecef0.x + 1, ecef0.y + 1, ecef0.z + 1}, &enu);

    EcefCoord ecef{};
    enu_to_ecef(sin_phi0, cos_phi0, sin_lambda0, cos_lambda0, ecef0, enu, &ecef);
    test_assert(almost_equal(ecef.x, ecef0.x + 1, EPSILON), "ecef.x wrong");
    test_assert(almost_equal(ecef.y, ecef0.y + 1, EPSILON), "ecef.y wrong");
    test_assert(almost_equal(ecef.z, ecef0.z + 1, EPSILON), "ecef.z wrong");
}

int main(int argc, char* argv[]) {
    test_geodetic_to_ecef();
    test_ecef_to_enu();
    test_geodetic_to_enu();
    test_enu_to_ecef();

    return 0;
}
