#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <random>

#include <glm/glm.hpp>

using Catch::Matchers::WithinAbs;

void checkVec3(glm::vec3 actual, glm::vec3 expected, double tol = 0.001)
{
    REQUIRE_THAT(actual.x, WithinAbs(expected.x, tol));
    REQUIRE_THAT(actual.y, WithinAbs(expected.y, tol));
    REQUIRE_THAT(actual.z, WithinAbs(expected.z, tol));
}

void checkVec4(glm::vec4 actual, glm::vec4 expected, double tol = 0.001)
{
    REQUIRE_THAT(actual.x, WithinAbs(expected.x, tol));
    REQUIRE_THAT(actual.y, WithinAbs(expected.y, tol));
    REQUIRE_THAT(actual.z, WithinAbs(expected.z, tol));
    REQUIRE_THAT(actual.w, WithinAbs(expected.w, tol));
}

std::string randomFileName()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<uint8_t> l(8, 16);
    std::uniform_int_distribution<uint8_t> u(97,122);
    std::string file;
    for (uint8_t i = 0; i < l(rng); i++)
    {
        file += char(u(rng));
    }
    return file;
}

#include <test_structure_input/test_structure_input.cpp>
#include <test_elements/test_elements.cpp>