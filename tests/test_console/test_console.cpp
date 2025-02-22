#include <console.h>
#include <visualisationState.h>

std::string Console::stackTrace("");
const float tol = 0.001;

VisualisationState vs;

SCENARIO("Lua interop")
{
    jLog::Log l;
    GIVEN("A Lua console")
    {
        Console console(l, &vs);
        WHEN("The script \"number = 3.14\" is run")
        {
            console.runString("number = 3.14");
            THEN("console.getGlobal<LuaNumber>(\"number\") == 3.14")
            {
                double num = console.getGlobal<LuaNumber>("number");
                REQUIRE_THAT(num, WithinAbs(3.14, tol));
            }
        }
        WHEN("The script \"bool = true\" is run")
        {
            console.runString("bool = true");
            THEN("console.getGlobal<LuaBool>(\"bool\") == true")
            {
                bool b = console.getGlobal<LuaBool>("bool");
                REQUIRE(b);
            }
        }
        WHEN("The script \"string = \"hello\"\" is run")
        {
            console.runString("string = \"hello\"");
            THEN("console.getGlobal<LuaString>(\"string\") == \"hello\"")
            {
                std::string s = console.getGlobal<LuaString>("string");
                REQUIRE(s==std::string("hello"));
            }
        }
        WHEN("The script \"array = {1.0,2.0,3.0}\" is run")
        {
            console.runString("array = {1.0,2.0,3.0}");
            THEN("console.getGlobal<LuaArray<3>>(\"array\") == {1.0,2.0,3.0}")
            {
                std::array<double, 3> a = console.getGlobal<LuaArray<3>>("array");
                REQUIRE_THAT(a[0], WithinAbs(1.0, tol));
                REQUIRE_THAT(a[1], WithinAbs(2.0, tol));
                REQUIRE_THAT(a[2], WithinAbs(3.0, tol));
            }
        }
        WHEN("The script \"vec = {1.0,2,3.0}\" is run")
        {
            console.runString("vec = {1.0,2,3.0}");
            THEN("console.getGlobal<LuaVec>(\"vec\") == {1.0,2.0,3.0}")
            {
                std::vector<double> a = console.getGlobal<LuaVec>("vec");
                REQUIRE_THAT(a[0], WithinAbs(1.0, tol));
                REQUIRE_THAT(a[1], WithinAbs(2.0, tol));
                REQUIRE_THAT(a[2], WithinAbs(3.0, tol));
            }
        }
        WHEN("The script \"table = {1.0,2,3.0}\" is run")
        {
            console.runString("table = {{1.0,2,3.0}}");
            THEN("console.getGlobal<LuaTable<LuaVec>>(\"table\") == {1.0,2.0,3.0}")
            {
                std::vector<LuaVec> a = console.getGlobal<LuaTable<LuaVec>>("table").data;
                REQUIRE_THAT(a[0][0], WithinAbs(1.0, tol));
                REQUIRE_THAT(a[0][1], WithinAbs(2.0, tol));
                REQUIRE_THAT(a[0][2], WithinAbs(3.0, tol));
            }
        }
        GIVEN("The VisualisationState has atomCount 1")
        {
            vs.atomCount = 1;
            WHEN("The script \"sfoav.setAtomColour(0, 0.0, 0.0, 0.0, 0.0)\" is run")
            {
                console.runString("sfoav.setAtomColour(0, 0.0, 0.0, 0.0, 0.0)");
                THEN("Then atomColourOverrides[0] is 0.0, 0.0, 0.0, 0.0")
                {
                    auto c = vs.atomColourOverrides[0];
                    REQUIRE_THAT(c[0], WithinAbs(0.0, tol));
                    REQUIRE_THAT(c[1], WithinAbs(0.0, tol));
                    REQUIRE_THAT(c[2], WithinAbs(0.0, tol));
                    REQUIRE_THAT(c[3], WithinAbs(0.0, tol));
                }
            }
            WHEN("The script \"sfoav.setAtomColour(0, 0.0, 0.0, 0.0, 0.0)\" is run")
            {
                console.runString("sfoav.setAtomColour(0, 2.0, 3.0, 4.0)");
                THEN("The atomColourOverrides[0] is 1.0, 1.0, 1.0, 1.0")
                {
                    auto c = vs.atomColourOverrides[0];
                    REQUIRE_THAT(c[0], WithinAbs(1.0, tol));
                    REQUIRE_THAT(c[1], WithinAbs(1.0, tol));
                    REQUIRE_THAT(c[2], WithinAbs(1.0, tol));
                    REQUIRE_THAT(c[3], WithinAbs(1.0, tol));
                }
            }
        }
    }
}