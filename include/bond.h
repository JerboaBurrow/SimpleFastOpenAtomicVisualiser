#ifndef BOND_H
#define BOND_H

#include <cstdint>
#include <map>

#include <constants.h>
#include <neighbours.h>

/**
 * @brief Obtain bonds based on a fixed distance cutOff using Neighbours.
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

    Neighbours n(atoms, 2.0f*cutOff);

    for (uint64_t i : forAtoms)
    {
        auto nn = n.neighbours(atoms, atoms[forAtoms[i]].position, cutOff);
        for (uint64_t j = 1; j < nn.size(); j++)
        {
            auto s = bonds[nn[j].first];
            if (s.find(i) == s.cend())
            {
                bonds[i].insert(nn[j].first);
            }
        }
    }
    return bonds;
}

#endif /* BOND_H */
