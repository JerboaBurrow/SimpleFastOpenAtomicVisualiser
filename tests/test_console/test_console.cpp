#include <console.h>
#include <util.h>
#include <visualisationState.h>

std::string Console::stackTrace("");
const float tol = 0.001;

std::vector<Atom> testAtoms = sfoavAtoms;

VisualisationState vs
(
    testAtoms,
    {},
    NULL_INDEX,
    1.0,
    keyCodes
);

SCENARIO("Lua atom colour interop")
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
        WHEN("The script \"sfoav.setAtomColour(0, 2.0, 3.0, 4.0)\" is run")
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
        WHEN("The script \"r, g, b, a = sfoav.getAtomColour(1)\"")
        {
            console.runString("r, g, b, a = sfoav.getAtomColour(1)");
            THEN("r is 1.0")
            {
                auto r = console.getGlobal<LuaNumber>("r");
                REQUIRE_THAT(r.n, WithinAbs(1.0, tol));
            }
            THEN("g is 0.784")
            {
                auto g = console.getGlobal<LuaNumber>("g");
                REQUIRE_THAT(g.n, WithinAbs(0.784, tol));
            }
            THEN("b is 0.196")
            {
                auto b = console.getGlobal<LuaNumber>("b");
                REQUIRE_THAT(b.n, WithinAbs(0.196, tol));
            }
            THEN("a is 1.0")
            {
                auto a = console.getGlobal<LuaNumber>("a");
                REQUIRE_THAT(a.n, WithinAbs(1.0, tol));
            }
        }
    }
}

SCENARIO("Lua bond interop")
{
    jLog::Log l;
    GIVEN("A Lua console (and no bonds)")
    {
        vs.bonds.clear();
        Console console(l, &vs);
        WHEN("The script \"empty = next(sfoav.getAtomsBonds(0))==nil\" is run")
        {
            console.runString("empty = next(sfoav.getAtomsBonds(0))==nil");
            THEN("empty is true")
            {
                auto a = console.getGlobal<LuaBool>("empty");
                REQUIRE(a.bit);
            }
        }
        WHEN("The script \"sfoav.bond(0, 4); n = sfoav.getAtomsBonds(0)[1]\" is run")
        {
            console.runString("sfoav.bond(0, 4); n = sfoav.getAtomsBonds(0)[1]");
            THEN("n is 4")
            {
                auto a = console.getGlobal<LuaNumber>("n");
                REQUIRE_THAT(a.n, WithinAbs(4.0, tol));
            }
            AND_THEN("4 is in bonds[0]")
            {
                REQUIRE(vs.bonds[0].find(4) != vs.bonds[0].cend());
            }
        }
        WHEN("The script \"sfoav.bond(0, 4); sfoav.unbond(4, 0); empty = next(sfoav.getAtomsBonds(0))==nil\" is run")
        {
            console.runString("sfoav.bond(0, 4); sfoav.unbond(4, 0); empty = next(sfoav.getAtomsBonds(0))==nil");
            THEN("empty is true")
            {
                auto a = console.getGlobal<LuaBool>("empty");
                REQUIRE(a.bit);
            }
            AND_THEN("4 is not in bonds[0]")
            {
                REQUIRE(vs.bonds[0].find(4) == vs.bonds[0].cend());
            }
        }
        WHEN("The script \"sfoav.bond(0, 4); sfoav.bond(0, 1); a = sfoav.getAtomsBonds(0)[1]; b = sfoav.getAtomsBonds(0)[2]\" is run")
        {
            console.runString("sfoav.bond(0, 4); sfoav.bond(0, 1); a = sfoav.getAtomsBonds(0)[1]; b = sfoav.getAtomsBonds(0)[2]");
            THEN("a is 4 and b is 1")
            {
                auto a = console.getGlobal<LuaNumber>("a");
                auto b = console.getGlobal<LuaNumber>("b");
                REQUIRE_THAT(a.n, WithinAbs(1.0, tol));
                REQUIRE_THAT(b.n, WithinAbs(4.0, tol));
            }
            AND_THEN("4 is in bonds[0]")
            {
                REQUIRE(vs.bonds[0].find(4) != vs.bonds[0].cend());
            }
            AND_THEN("1 is in bonds[0]")
            {
                REQUIRE(vs.bonds[0].find(1) != vs.bonds[0].cend());
            }
        }
    }
}