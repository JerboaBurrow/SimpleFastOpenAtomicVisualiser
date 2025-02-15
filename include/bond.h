#ifndef BOND_H
#define BOND_H

#include <cstdint>

#include <constants.h>

/**
 * @brief A Bond structure.
 *
 * @remark A Bond refers to indices in some std::vector<Atoms>, @see Atom.
 * This mean heavy data (positions etc.) is only stored once.
 */
struct Bond
{

    Bond(uint64_t a, uint64_t b)
    : atomIndexA(a), atomIndexB(b)
    {}

    /**
     * @brief Index of first Atom in Bond
     *
     */
    uint64_t atomIndexA;

    /**
     * @brief Index of second Atom in Bond
     *
     */
    uint64_t atomIndexB;
};

/**
 * @brief Obtain bonds based on a fixed distance cutOff.
 *
 * @param forAtoms the atoms to find bonds for.
 * @param atoms the Atoms to bond.
 * @param cutOff the distance cutoff below which Atoms are bonded.
 * @return std::vector<Bond> the resulting Bonds.
 * @remark Currently a naive direct distance evaluation.
 */
std::vector<Bond> determineBonds
(
    std::vector<uint64_t> forAtoms,
    std::vector<Atom> & atoms,
    float cutOff
)
{
    if (cutOff <= 0.0f || forAtoms.size() == 0) { return {}; }

    std::vector<Bond> bonds;
    bonds.reserve(atoms.size());

    for (uint64_t i : forAtoms)
    {
        for (uint64_t j = 0; j < atoms.size(); j++)
        {
            if (j != i && glm::length(atoms[j].position-atoms[i].position) <= cutOff)
            {
                bonds.push_back({i, j});
            }
        }
    }
    return bonds;
}

#endif /* BOND_H */
