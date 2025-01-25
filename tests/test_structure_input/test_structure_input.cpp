#include <config.h>
#include <xyz.h>

SCENARIO("XYZ reading")
{
    GIVEN("psilocybin.xyz")
    {
        THEN("ostensiblyXYZLike(\"psilocybin.xyz\") is true")
        {
            REQUIRE(ostensiblyXYZLike("psilocybin.xyz"));
        }
        WHEN("An XYZ, xyz, is instanced with it")
        {
            XYZ xyz("psilocybin.xyz");
            THEN("xyz has 36 atoms")
            {
                REQUIRE(xyz.atomCount() == 36);
            }
            WHEN("A frame is obtained")
            {
                auto frame = xyz.readFrame(0);
                THEN("There are 36 Atoms")
                {
                    REQUIRE(frame.size() == 36);
                }
                THEN("The first atom is Element::C with position [11.18383, 7.74617, 4.00055]")
                {
                    frame[0].symbol = Element::C;
                    frame[0].position = glm::vec3(11.18383, 7.74617, 4.00055);
                }
            }
        }
    }
}

SCENARIO("CONFIG reading")
{
    GIVEN("CONFIG")
    {
        THEN("ostensiblyXYZLike(\"CONFIG\") is false")
        {
            REQUIRE(!ostensiblyXYZLike("CONFIG"));
        }
        WHEN("A CONFIG, config, is instanced with it")
        {
            CONFIG config("CONFIG");
            THEN("config has 100 atoms")
            {
                REQUIRE(config.atomCount() == 100);
            }
            THEN("config has 1 frame")
            {
                REQUIRE(config.frameCount() == 1);
            }
            THEN("config has cell vector A [17.3952969480, 0.0000000000, 0.0000000000]")
            {
                REQUIRE(config.getCellA() == glm::vec3(17.3952969480, 0.0000000000, 0.0000000000));
            }
            THEN("config has cell vector B [0.0000000000, 17.3952969480, 0.0000000000]")
            {
                REQUIRE(config.getCellB() == glm::vec3(0.0000000000, 17.3952969480, 0.0000000000));
            }
            THEN("config has cell vector C [0.0000000000, 0.0000000000, 17.3952969480]")
            {
                REQUIRE(config.getCellC() == glm::vec3(0.0000000000, 0.0000000000, 17.3952969480));
            }
            WHEN("A frame is obtained")
            {
                auto frame = config.readFrame(0);
                THEN("There are 100 Atoms")
                {
                    REQUIRE(frame.size() == 100);
                }
                THEN("The first atom is Element::Ar with position [11.18383, 7.74617, 4.00055]")
                {
                    frame[0].symbol = Element::Ar;
                    frame[0].position = glm::vec3(4.023972884, 2.257201511, 2.476523008);
                }
                THEN("The first atom has velocity [0.7345153305, 1.682592971, 1.434504742]")
                {
                    frame[0].position = glm::vec3(0.7345153305, 1.682592971, 1.434504742);
                }
                THEN("The first atom has force [-808.1553733, -36.32789404, 156.6931151]")
                {
                    frame[0].position = glm::vec3(-808.1553733, -36.32789404, 156.6931151);
                }
                THEN("config is at frame 1")
                {
                    REQUIRE(config.framePosition() == 1);
                }
            }
        }
    }
}

SCENARIO("REVCON reading")
{
    GIVEN("REVCON")
    {
        THEN("ostensiblyXYZLike(\"REVCON\") is false")
        {
            REQUIRE(!ostensiblyXYZLike("REVCON"));
        }
        WHEN("A CONFIG, revcon, is instanced with it")
        {
            CONFIG revcon("REVCON");
            THEN("revcon has 1024 atoms")
            {
                REQUIRE(revcon.atomCount() == 1024);
            }
            THEN("revcon has 1 frame")
            {
                REQUIRE(revcon.frameCount() == 1);
            }
            THEN("revcon has cell vector A [37.2652972799, 0.0000000000, 0.0000000000]")
            {
                REQUIRE(revcon.getCellA() == glm::vec3(37.2652972799, 0.0000000000, 0.0000000000));
            }
            THEN("revcon has cell vector B [0.0000000000, 16.9395910291, 0.0000000000]")
            {
                REQUIRE(revcon.getCellB() == glm::vec3(0.0000000000, 16.9395910291, 0.0000000000));
            }
            THEN("revcon has cell vector C [0.0000000000, 0.0000000000, 16.9395910291]")
            {
                REQUIRE(revcon.getCellC() == glm::vec3(0.0000000000, 0.0000000000, 16.9395910291));
            }
            WHEN("A frame is obtained")
            {
                auto frame = revcon.readFrame(0);
                THEN("There are 1024 Atoms")
                {
                    REQUIRE(frame.size() == 1024);
                }
                THEN("The first atom is Element::Li with position [1.364179598, 5.261643835, -3.555355886]")
                {
                    frame[0].symbol = Element::Li;
                    frame[0].position = glm::vec3(1.364179598, 5.261643835, -3.555355886);
                }
                THEN("The first atom has velocity [-30.19359237, 1.161642755, -14.68627930]")
                {
                    frame[0].position = glm::vec3(-30.19359237, 1.161642755, -14.68627930);
                }
                THEN("The first atom has force [1129.538890, -2548.505399, -4894.155164]")
                {
                    frame[0].position = glm::vec3(1129.538890, -2548.505399, -4894.155164);
                }
                THEN("revcon is at frame 1")
                {
                    REQUIRE(revcon.framePosition() == 1);
                }
            }
        }
    }
}

SCENARIO("HISTORY reading")
{
    GIVEN("HISTORY")
    {
        THEN("ostensiblyXYZLike(\"HISTORY\") is false")
        {
            REQUIRE(!ostensiblyXYZLike("HISTORY"));
        }
        WHEN("A CONFIG, history, is instanced with it")
        {
            CONFIG history("HISTORY");
            THEN("history has 320 atoms")
            {
                REQUIRE(history.atomCount() == 320);
            }
            THEN("history has 11 frames")
            {
                REQUIRE(history.frameCount() == 11);
            }
            THEN("history has cell vector A [14.1109800000, 0.0000000000, 0.0000000000]")
            {
                REQUIRE(history.getCellA() == glm::vec3(14.1109800000, 0.0000000000, 0.0000000000));
            }
            THEN("history has cell vector B [0.0000000000, 14.1109800000, 0.0000000000]")
            {
                REQUIRE(history.getCellB() == glm::vec3(0.0000000000, 14.1109800000, 0.0000000000));
            }
            THEN("history has cell vector C [0.0000000000, 0.0000000000, 14.1109800000]")
            {
                REQUIRE(history.getCellC() == glm::vec3(0.0000000000, 0.0000000000, 14.1109800000));
            }
            WHEN("A frame is obtained")
            {
                auto frame = history.readFrame(0);
                THEN("There are 320 Atoms")
                {
                    REQUIRE(frame.size() == 320);
                }
                THEN("The first atom is Element::C with position [1.088900000, 1.088900000, 1.088900000]")
                {
                    frame[0].symbol = Element::C;
                    frame[0].position = glm::vec3(1.088900000, 1.088900000, 1.088900000);
                }
                THEN("history is at frame 1")
                {
                    REQUIRE(history.framePosition() == 1);
                }
            }
        }
    }
}