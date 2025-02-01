#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <glm/glm.hpp>

using Catch::Matchers::WithinAbs;

void checkVec3(glm::vec3 actual, glm::vec3 expected, double tol = 0.001)
{
    REQUIRE_THAT(actual.x, WithinAbs(expected.x, tol));
    REQUIRE_THAT(actual.y, WithinAbs(expected.y, tol));
    REQUIRE_THAT(actual.z, WithinAbs(expected.z, tol));
}

#include <test_structure_input/test_structure_input.cpp>