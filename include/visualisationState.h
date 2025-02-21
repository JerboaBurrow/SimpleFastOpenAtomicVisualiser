#ifndef VISUALISATIONSTATE_H
#define VISUALISATIONSTATE_H

#include <vector>
#include <map>
#include <cstdint>

#include <GLFW/glfw3.h>

#include <colour.h>
#include <bond.h>
#include <atom.h>
#include <element.h>

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

        applyColours(atoms, atomColourOverrides);

        if (!atomColours.empty())
        {
            atomColourOverrides = atomColoursFromFile(atomColours);
        }

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
    }

    std::vector<Bond> bonds;
    std::vector<uint64_t> bondsFor;
    std::vector<float> atomEmphasisOverrides;
    std::map<uint64_t, glm::vec4> atomColourOverrides;
    std::multimap<Element, uint64_t> elementMap;
    std::map<int, Element> emphasisControls;

};

#endif /* VISUALISATIONSTATE_H */
