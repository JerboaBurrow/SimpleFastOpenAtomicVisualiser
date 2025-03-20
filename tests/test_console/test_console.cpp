#include <console.h>
#include <util.h>
#include <visualisationState.h>

std::string Console::stackTrace("");

std::vector<Atom> testAtoms = sfoavAtoms;

VisualisationState vs
(
    testAtoms,
    {},
    NULL_INDEX,
    1.0,
    false,
    keyCodes
);

CommandLine options;
Camera camera(testAtoms, 64, 64);

SCENARIO("Lua atom interop")
{
    jLog::Log l;
    GIVEN("A Lua console")
    {
        Console console(l, &vs, &options, &camera);
        WHEN("The script \"atoms = sfoav.atomCount()\" is run")
        {
            console.runString("atoms = sfoav.atomCount()");
            THEN("atoms is 58")
            {
                auto atoms = console.getGlobal<LuaNumber>("atoms");
                REQUIRE(atoms.n == 58);
            }
        }
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
        WHEN("The script \"element = sfoav.getAtom(0)[\"element\"]\"")
        {
            console.runString("element = sfoav.getAtom(0)[\"element\"]");
            THEN("element is \"S\"")
            {
                auto s = console.getGlobal<LuaString>("element");
                REQUIRE(s.characters == "S");
            }
        }
        WHEN("The script \"x = sfoav.getAtom(0)[\"position\"][\"x\"]\"")
        {
            console.runString("x = sfoav.getAtom(0)[\"position\"][\"x\"]");
            THEN("x is -11.3966")
            {
                auto a = console.getGlobal<LuaNumber>("x");
                REQUIRE_THAT(a.n, WithinAbs(-11.3966, tol));
            }
        }
        WHEN("The script \"y = sfoav.getAtom(0)[\"position\"][\"y\"]\"")
        {
            console.runString("y = sfoav.getAtom(0)[\"position\"][\"y\"]");
            THEN("y is -2.10345")
            {
                auto a = console.getGlobal<LuaNumber>("y");
                REQUIRE_THAT(a.n, WithinAbs(-2.10345, tol));
            }
        }
        WHEN("The script \"z = sfoav.getAtom(0)[\"position\"][\"z\"]\"")
        {
            console.runString("z = sfoav.getAtom(0)[\"position\"][\"z\"]");
            THEN("z is 0.0")
            {
                auto a = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(a.n, WithinAbs(0.0, tol));
            }
        }
        WHEN("The script \"radius = sfoav.getAtom(0)[\"radius\"]\"")
        {
            console.runString("radius = sfoav.getAtom(0)[\"radius\"]");
            THEN("radius is 1.89f")
            {
                auto a = console.getGlobal<LuaNumber>("radius");
                REQUIRE_THAT(a.n, WithinAbs(1.89f, tol));
            }
        }
        WHEN("The script \"element = sfoav.getAtom(12)[\"element\"]\"")
        {
            console.runString("element = sfoav.getAtom(12)[\"element\"]");
            THEN("element is \"F\"")
            {
                auto s = console.getGlobal<LuaString>("element");
                REQUIRE(s.characters == "F");
            }
        }
        WHEN("The script \"x = sfoav.getAtom(12)[\"position\"][\"x\"]\"")
        {
            console.runString("x = sfoav.getAtom(12)[\"position\"][\"x\"]");
            THEN("x is -6.39655")
            {
                auto a = console.getGlobal<LuaNumber>("x");
                REQUIRE_THAT(a.n, WithinAbs(-6.39655, tol));
            }
        }
        WHEN("The script \"g = sfoav.getAtom(12)[\"colour\"][\"g\"]\"")
        {
            console.runString("g = sfoav.getAtom(12)[\"colour\"][\"g\"]");
            THEN("g is 0.647")
            {
                auto a = console.getGlobal<LuaNumber>("g");
                REQUIRE_THAT(a.n, WithinAbs(0.647, tol));
            }
        }
        WHEN("The velocity of 12 is set to glm::vec3(1.0f)")
        {
            testAtoms[12].velocity = glm::vec3(1.0f);
            AND_WHEN("The script \"vx = sfoav.getAtom(12)[\"velocity\"][\"x\"]\"")
            {
                console.runString("vx = sfoav.getAtom(12)[\"velocity\"][\"x\"]");
                THEN("vx is 1.0")
                {
                    auto a = console.getGlobal<LuaNumber>("vx");
                    REQUIRE_THAT(a.n, WithinAbs(1.0, tol));
                }
            }
        }
        WHEN("The force of 12 is set to glm::vec3(1.0f)")
        {
            testAtoms[12].force = glm::vec3(1.0f);
            AND_WHEN("The script \"fx = sfoav.getAtom(12)[\"force\"][\"x\"]\"")
            {
                console.runString("fx = sfoav.getAtom(12)[\"force\"][\"x\"]");
                THEN("fx is 1.0")
                {
                    auto a = console.getGlobal<LuaNumber>("fx");
                    REQUIRE_THAT(a.n, WithinAbs(1.0, tol));
                }
            }
        }
        WHEN("The script \"radius = sfoav.getAtom(12)[\"radius\"]\" is run")
        {
            console.runString("radius = sfoav.getAtom(12)[\"radius\"]");
            THEN("radius is 1.46f")
            {
                auto a = console.getGlobal<LuaNumber>("radius");
                REQUIRE_THAT(a.n, WithinAbs(1.46f, tol));
            }
        }
        WHEN("The script \"n = sfoav.getAtomsNeighbours(0, 3.14)[2][\"index\"]\" is run")
        {
            console.runString("n = sfoav.getAtomsNeighbours(0, 3.14)[2][\"index\"]");
            Neighbours n(testAtoms);
            const auto directNeighbours = n.neighboursDirect(testAtoms, testAtoms[0].position, 3.14f);
            THEN("n is equal to the second neighbour's index")
            {
                uint64_t n = console.getGlobal<LuaNumber>("n").n;
                REQUIRE(n == directNeighbours[1].first);
            }
        }
        WHEN("The script \"d = sfoav.getAtomsNeighbours(0, 3.14)[2][\"distance\"]\" is run")
        {
            console.runString("d = sfoav.getAtomsNeighbours(0, 3.14)[2][\"distance\"]");
            Neighbours n(testAtoms);
            const auto directNeighbours = n.neighboursDirect(testAtoms, testAtoms[0].position, 3.14f);
            THEN("d is equal to the second neighbour's distance")
            {
                float n = console.getGlobal<LuaNumber>("d").n;
                REQUIRE_THAT(n, WithinAbs(directNeighbours[1].second, tol));
            }
        }
        WHEN("The script \"d = sfoav.getAtomsNeighbours(0, 3.14, false)[2][\"distance\"]\" is run")
        {
            console.runString("d = sfoav.getAtomsNeighbours(0, 3.14, false)[2][\"distance\"]");
            Neighbours n(testAtoms);
            const auto directNeighbours = n.neighboursDirect(testAtoms, testAtoms[0].position, 3.14f, false);
            THEN("d is equal to the second neighbour's distance")
            {
                float n = console.getGlobal<LuaNumber>("d").n;
                REQUIRE_THAT(n, WithinAbs(directNeighbours[1].second, tol));
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
        Console console(l, &vs, &options, &camera);
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

SCENARIO("Lua camera interop")
{
    jLog::Log l;
    GIVEN("A Lua console and a camera at spherical coordinates (1,3.14,1.57)")
    {
        Console console(l, &vs, &options, &camera);
        camera.setPosition({1,3.14,1.57});
        WHEN("The script \"x, y, z = sfoav.cameraPosition(true)\"")
        {
            console.runString("x, y, z = sfoav.cameraPosition(true)");
            THEN("x, y, and z are 1, 3.14, and 1.57 ")
            {
                auto x = console.getGlobal<LuaNumber>("x");
                auto y = console.getGlobal<LuaNumber>("y");
                auto z = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(x.n, WithinAbs(1.0, 0.01));
                REQUIRE_THAT(y.n, WithinAbs(3.14, 0.01));
                REQUIRE_THAT(z.n, WithinAbs(1.57, 0.01));
            }
        }
        WHEN("The script \"sfoav.setCameraPosition(1.0, 0.0, 0.0); x, y, z = sfoav.cameraPosition(false)\"")
        {
            console.runString("x, y, z = sfoav.cameraPosition(false)");
            THEN("x, y, and z are 1, 3.14, and 1.57 ")
            {
                auto x = console.getGlobal<LuaNumber>("x");
                auto y = console.getGlobal<LuaNumber>("y");
                auto z = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(x.n, WithinAbs(0.0, 0.01));
                REQUIRE_THAT(y.n, WithinAbs(-1.0, 0.01));
                REQUIRE_THAT(z.n, WithinAbs(0.0, 0.01));
            }
        }
        WHEN("The script \"sfoav.inclineCamera(-1.0); x, y, z = sfoav.cameraPosition(true)\"")
        {
            console.runString("sfoav.inclineCamera(-1.0); x, y, z = sfoav.cameraPosition(true)");
            THEN("x, y, and z are 1, 2.14, and 1.57 ")
            {
                auto x = console.getGlobal<LuaNumber>("x");
                auto y = console.getGlobal<LuaNumber>("y");
                auto z = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(x.n, WithinAbs(1.0, 0.01));
                REQUIRE_THAT(y.n, WithinAbs(2.14, 0.01));
                REQUIRE_THAT(z.n, WithinAbs(1.57, 0.01));
            }
        }
        WHEN("The script \"sfoav.rotateCamera(-1.0); x, y, z = sfoav.cameraPosition(true)\"")
        {
            console.runString("sfoav.rotateCamera(-1.0); x, y, z = sfoav.cameraPosition(true)");
            THEN("x, y, and z are 1, 3.14, and 0.57 ")
            {
                auto x = console.getGlobal<LuaNumber>("x");
                auto y = console.getGlobal<LuaNumber>("y");
                auto z = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(x.n, WithinAbs(1.0, 0.01));
                REQUIRE_THAT(y.n, WithinAbs(3.14, 0.01));
                REQUIRE_THAT(z.n, WithinAbs(0.57, 0.01));
            }
        }
        WHEN("The script \"sfoav.zoomCamera(-1.0); x, y, z = sfoav.cameraPosition(true)\"")
        {
            console.runString("sfoav.zoomCamera(-1.0); x, y, z = sfoav.cameraPosition(true)");
            THEN("x, y, and z are 2, 3.14, and 1.57 ")
            {
                auto x = console.getGlobal<LuaNumber>("x");
                auto y = console.getGlobal<LuaNumber>("y");
                auto z = console.getGlobal<LuaNumber>("z");
                REQUIRE_THAT(x.n, WithinAbs(2.0, 0.01));
                REQUIRE_THAT(y.n, WithinAbs(3.14, 0.01));
                REQUIRE_THAT(z.n, WithinAbs(1.57, 0.01));
            }
        }
    }
}