#include <util.h>
#include <neighbours.h>

SCENARIO("Neighbours")
{
    GIVEN("sfoavAtoms Atoms")
    {
        WHEN("A Neighbours is constructed with them")
        {
            Neighbours n(sfoavAtoms);
            const auto directNeighbours = n.neighboursDirect(sfoavAtoms, sfoavAtoms[0].position, 1000.0f, false);
            THEN("The neighbours of sfoavAtoms[0].position to 0.1f is 0")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 0.1f, false, false);
                REQUIRE(v.size()==1);
                REQUIRE(v[0].first == 0);
            }
            THEN("The neighbours of sfoavAtoms[0].position to 1000.0f (with noDirect = true) is equivalent to a direct distance calculation")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 1000.0f, true, false);
                REQUIRE(v.size() == directNeighbours.size());
                for (uint64_t i = 0; i < v.size(); i++)
                {
                    REQUIRE(v[i].first == directNeighbours[i].first);
                    REQUIRE_THAT(v[i].second, WithinAbs(directNeighbours[i].second, tol));
                }
            }
            THEN("The neighbours of sfoavAtoms[0].position to 3.14f (with noDirect = true) are equivalent to the direct calculation.")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 3.14f, true, false);
                std::vector<std::pair<uint64_t, float>>  n{};
                for (uint64_t i = 0; i < directNeighbours.size(); i++)
                {
                    if (directNeighbours[i].second <= 3.14f) { n.push_back(directNeighbours[i]); }
                }
                REQUIRE(v.size() == n.size());
                for (uint64_t i = 0; i < v.size(); i++)
                {
                    REQUIRE(v[i].first == n[i].first);
                    REQUIRE_THAT(v[i].second, WithinAbs(n[i].second, tol));
                }
            }
        }
    }
}

SCENARIO("Neighbours with nearest images")
{
    GIVEN("sfoavAtoms Atoms")
    {
        WHEN("A Neighbours is constructed with them")
        {
            Neighbours n(sfoavAtoms);
            const auto directNeighbours = n.neighboursDirect(sfoavAtoms, sfoavAtoms[0].position, 1000.0f);
            THEN("The neighbours of sfoavAtoms[0].position to 0.1f is 0")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 0.1f);
                REQUIRE(v.size()==1);
                REQUIRE(v[0].first == 0);
            }
            THEN("The neighbours of sfoavAtoms[0].position to 1000.0f (with noDirect = true) is equivalent to a direct distance calculation")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 1000.0f, true);
                REQUIRE(v.size() == directNeighbours.size());
                for (uint64_t i = 0; i < v.size(); i++)
                {
                    REQUIRE(v[i].first == directNeighbours[i].first);
                    REQUIRE_THAT(v[i].second, WithinAbs(directNeighbours[i].second, tol));
                }
            }
            THEN("The neighbours of sfoavAtoms[0].position to 3.14f (with noDirect = true) are equivalent to the direct calculation.")
            {
                auto v = n.neighbours(sfoavAtoms, sfoavAtoms[0].position, 3.14f, true);
                std::vector<std::pair<uint64_t, float>>  n{};
                for (uint64_t i = 0; i < directNeighbours.size(); i++)
                {
                    if (directNeighbours[i].second <= 3.14f) { n.push_back(directNeighbours[i]); }
                }
                REQUIRE(v.size() == n.size());
                for (uint64_t i = 0; i < v.size(); i++)
                {
                    REQUIRE(v[i].first == n[i].first);
                    REQUIRE_THAT(v[i].second, WithinAbs(n[i].second, tol));
                }
            }
        }
    }
}