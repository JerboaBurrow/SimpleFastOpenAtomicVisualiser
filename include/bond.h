#ifndef BOND_H
#define BOND_H

#include <cstdint>
#include <map>

#include <constants.h>

/**
 * @brief Obtain bonds based on a fixed distance cutOff.
 *
 * @param forAtoms the atoms to find bonds for.
 * @param atoms the Atoms to bond.
 * @param cutOff the distance cutoff below which Atoms are bonded.
 * @return std::map<uint64_t, std::set<uint64_t>> the resulting Bonds.
 * @remark Currently a naive direct distance evaluation.
 */
std::map<uint64_t, std::set<uint64_t>> determineBonds
(
    std::vector<uint64_t> forAtoms,
    std::vector<Atom> & atoms,
    float cutOff
)
{
    if (cutOff <= 0.0f || forAtoms.size() == 0) { return {}; }

    std::map<uint64_t, std::set<uint64_t>> bonds;

    for (uint64_t i : forAtoms)
    {
        for (uint64_t j = 0; j < atoms.size(); j++)
        {
            if (j != i && glm::length(atoms[j].position-atoms[i].position) <= cutOff)
            {
                auto s = bonds[j];
                if (s.find(i) == s.cend())
                {
                    bonds[i].insert(j);
                }
            }
        }
    }
    return bonds;
}

#endif /* BOND_H */
