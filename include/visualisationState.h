#ifndef VISUALISATIONSTATE_H
#define VISUALISATIONSTATE_H

#include <vector>
#include <map>
#include <cstdint>
#include <string>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <lua.h>

#include <colour.h>
#include <bond.h>
#include <atom.h>
#include <element.h>
#include <LuaNumber.h>

/**
 * @brief Holds editable data for the visualisation state.
 *
 */
struct VisualisationState
{

    /**
     * @brief Construct an empty VisualisationState.
     *
     */
    VisualisationState()
    {}

    /**
     * @brief Construct a VisualisationState from a some Atoms.
     *
     * @param atoms the Atoms to construct from.
     * @param atomColours file path for colour overrides.
     * @param bondFocus a particular atom to focus bonds on.
     * @param bondCutoff a bond cutoff distance.
     * @param keyCodes key codes.
     */
    VisualisationState
    (
        std::vector<Atom> & atoms,
        const std::filesystem::path & atomColours,
        uint64_t bondFocus,
        float bondCutoff,
        const std::map<int, std::string> & keyCodes
    )
    {
        std::set<Element> elements = uniqueElements(atoms);
        elementMap = elementIndices(atoms);

        atomEmphasisOverrides = std::vector<float>(atoms.size(), 1.0f);

        for (uint8_t i = 0; i < std::min(size_t(6), elements.size()); i++)
        {
            Element e = *std::next(elements.begin(), i);
            emphasisControls[GLFW_KEY_1+i] = e;
            std::cout << "Element " << e << " emphasis bound to key " << keyCodes.at(GLFW_KEY_1+i) << "\n";
        }

        if (!atomColours.empty())
        {
            atomColourOverrides = atomColoursFromFile(atomColours);
        }

        applyColours(atoms, atomColourOverrides);

        if (bondFocus < atoms.size())
        {
            bondsFor = {bondFocus};
        }
        else
        {
            bondsFor.resize(atoms.size());
            std::iota(bondsFor.begin(), bondsFor.end(), 0);
        }

        if (bondCutoff > 0.0)
        {
            bonds = determineBonds
            (
                bondsFor,
                atoms,
                bondCutoff
            );
        }
        atomCount = atoms.size();
    }

    std::vector<Bond> bonds;
    std::vector<uint64_t> bondsFor;
    std::vector<float> atomEmphasisOverrides;
    std::map<uint64_t, glm::vec4> atomColourOverrides;
    std::multimap<Element, uint64_t> elementMap;
    std::map<int, Element> emphasisControls;

    uint64_t atomCount;

    /**
     * @brief Lua binding to set an Atoms colour by index.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * 2. The red channel [0, 1].
     * 3. The green channel [0, 1].
     * 4. The blue channel [0, 1].
     * 5. [Optional] The alpha channel [0, 1] defaults to 1.
     * Call as sfoav.setAtomColour(5, 1.0, 0.5, 0.5, 1.0)
     * @param lua the Lua context.
     * @return int the return code.
     */
    int lua_setAtomColour(lua_State * lua)
    {
        int args = lua_gettop(lua);
        if (args < 4 || args > 5)
        {
            const std::string msg = "setAtomColour expects an atom index and RGB or RGBA arguments.\n";
            lua_pushlstring(lua, msg.c_str(), msg.length());
            return lua_error(lua);
        }
        LuaNumber lua_index, lua_r, lua_g, lua_b, lua_a;
        uint64_t index;
        float r, g, b, a;
        a = 1.0f;

        lua_index.read(lua, 1);
        index = uint64_t(lua_index.n);

        if (index >= atomCount)
        {
            const std::string msg = "setAtomColour atom index larger than atom count.\n";
            lua_pushlstring(lua, msg.c_str(), msg.length());
            return lua_error(lua);
        }
        lua_r.read(lua, 2);
        r = std::clamp(float(lua_r.n), 0.0f, 1.0f);
        lua_g.read(lua, 3);
        g = std::clamp(float(lua_g.n), 0.0f, 1.0f);
        lua_b.read(lua, 4);
        b = std::clamp(float(lua_b.n), 0.0f, 1.0f);
        if (args == 5)
        {
            lua_a.read(lua, 5);
            a = std::clamp(float(lua_a.n), 0.0f, 1.0f);
        }
        atomColourOverrides[index] = glm::vec4(r, g, b, a);

        return 0;
    }

};

#endif /* VISUALISATIONSTATE_H */
