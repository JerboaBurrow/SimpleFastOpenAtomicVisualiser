#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include <constants.h>
#include <atom.h>

/**
 * @brief Calculate neighbour lists.
 *
 * @remark spatial hashing based upon a fixed grid.
 */
class Neighbours
{

public:

    /**
     * @brief Construct a Neighbour list from some Atoms.
     *
     * @param atoms the Atoms to hash into spatial domains.
     */
    Neighbours
    (
        const std::vector<Atom> & atoms,
        float domainSideLength = -1.0
    )
    {
        if (domainSideLength == -1.0)
        {
            float r = largest(atoms);
            domainSize = 2.0f*glm::vec3(r, r, r);
        }
        else
        {
            domainSize = glm::vec3(domainSideLength, domainSideLength, domainSideLength);
        }

        minPoint = min(atoms);
        length = max(atoms)-minPoint;


        length += domainSize;

        nx = length.x / domainSize.x;
        ny = length.y / domainSize.y;
        nz = length.z / domainSize.z;

        clear();

        build(atoms);
    }

    /**
     * @brief Build the neighbour list from some Atoms.
     *
     * @param atoms the Atoms to hash into spatial domains.
     */
    void build(const std::vector<Atom> & atoms)
    {
        domainAssignments = std::vector<uint64_t>(atoms.size(), NULL_INDEX);
        for (uint64_t i = 0; i < atoms.size(); i++)
        {
            domainAssignments[i] = std::min(hash(atoms[i].position), uint64_t(domains.size()-1));
            domains[domainAssignments[i]].push_back(i);
        }
    }

    /**
     * @brief Reset the spatial domains.
     *
     */
    void clear()
    {
        // Set to twice close packing domain list size.
        // Overlaps may lead to > close packed size.
        domains = std::vector<std::vector<uint64_t>>(nx*ny*nz);
        for (auto & domain : domains)
        {
            domain.reserve(2*std::ceil(domainSize.x*domainSize.y*domainSize.z*0.74048));
        }
        domainAssignments = std::vector<uint64_t>(domainAssignments.size(), NULL_INDEX);
    }

    /**
     * @brief Get the neighbours to a position within a cutoff using the spatial domains.
     *
     * @param atoms the Atoms passed to Neighbours::build.
     * @param position the position to find neighbours to.
     * @param cutoff the spatial cutoff.
     * @param noDirect force use of the spatial partitioning.
     * @param nearestImage whether to use the nearest image convention.
     * @return std::vector<std::pair<uint64_t, float>> the indices of Atom
     * neighbours to position and distances. In ascending distance order.
     *
     * @remark The Atoms are passed as reference here and in Neighbours::build to
     * avoid copying data. If different Atoms are passed in each distances will be
     * wrong. If more Atoms are passed those with higher indices are ignored.
     * @remark Atoms outside of bounds are ignored.
     * @remark Unless noDirect is true. If the cutoff > the half-width of the spatial partitioning, a direct
     * evaluation is performed.
     * @remark Distance sorting ties are solved by atom index.
     */
    std::vector<std::pair<uint64_t, float>> neighbours
    (
        const std::vector<Atom> & atoms,
        glm::vec3 position,
        float cutoff,
        bool noDirect = false,
        bool nearestImage = true
    ) const
    {
        std::map<uint64_t, float> n;

        auto wrap = [](int i, int m){ return ((i % m) + m) % m; };

        auto coords = domainCoords(position);
        float rc2 = cutoff*cutoff;
        float halfWidth = 0.5f*std::min(std::min(length.x, length.y), length.z);
        if (!noDirect && cutoff >= halfWidth)
        {
            return neighboursDirect(atoms, position, cutoff, nearestImage);
        }

        cutoff = std::min(cutoff, std::max(std::max(length.x, length.y), length.z));

        // Shells to search in either direction.
        int sx = int(std::ceil(cutoff/domainSize.x))+1;
        int sy = int(std::ceil(cutoff/domainSize.y))+1;
        int sz = int(std::ceil(cutoff/domainSize.z))+1;

        for (int i = -sx; i <= sx; i++)
        {
            uint64_t u = (wrap(i+int(coords.x), nx))*ny*nz;
            for (int j = -sy; j <= sy; j++)
            {
                uint64_t v = (wrap(j+int(coords.y), ny))*nz;
                for (int k = -sz; k <= sz; k++)
                {
                    uint64_t w = u+v+wrap(k+int(coords.z), nz);
                    if (w > domains.size()) { continue; }
                    for (const auto & index : domains[w])
                    {
                        if (index > atoms.size()) { break; }
                        glm::vec3 r = position-atoms[index].position;
                        if (nearestImage)
                        {
                            for (uint8_t c = 0; c < 3; c++)
                            {
                                if (r[c] > length[c]*0.5f) { r[c] = r[c]-length[c]; }
                                if (r[c] <= -length[c]*0.5f) { r[c] = r[c]+length[c]; }
                            }
                        }
                        float d2 = glm::dot(r, r);
                        if (d2 <= rc2)
                        {
                            n.insert({index, std::sqrt(d2)});
                        }
                    }
                }
            }
        }
        std::vector<std::pair<uint64_t, float>> nd(n.begin(), n.end());
        std::sort
        (
            nd.begin(),
            nd.end(),
            []
            (
                const std::pair<uint64_t, float> & a,
                const std::pair<uint64_t, float> & b
            ) { return a.second == b.second ? a.first < b.first : a.second < b.second; }
        );
        return nd;
    }

    /**
     * @brief Get the neighbours to a position within a cutoff by a direct evaluation.
     *
     * @param atoms the Atoms passed to Neighbours::build.
     * @param position the position to find neighbours to.
     * @param cutoff the spatial cutoff.
     * @param nearestImage whether to use the nearest image convention.
     * @return std::vector<std::pair<uint64_t, float>> the indices of Atom
     * neighbours to position and distances. In ascending distance order.
     *
     * @remark A direct O(atoms.size()) comparision is performed.
     * @remark Distance sorting ties are solved by atom index.
     */
    std::vector<std::pair<uint64_t, float>> neighboursDirect
    (
        const std::vector<Atom> & atoms,
        glm::vec3 position,
        float cutoff,
        bool nearestImage = true
    ) const
    {
        std::vector<std::pair<uint64_t, float>> directNeighbours;
        float rc2 = cutoff*cutoff;
        for (uint64_t i = 0; i < atoms.size(); i++)
        {
            glm::vec3 r = position-atoms[i].position;
            if (nearestImage)
            {
                for (uint8_t c = 0; c < 3; c++)
                {
                    if (r[c] > length[c]*0.5f) { r[c] = r[c]-length[c]; }
                    if (r[c] <= -length[c]*0.5f) { r[c] = r[c]+length[c]; }
                }
            }
            float d2 = glm::dot(r, r);
            if (d2 <= rc2)
            {
                directNeighbours.push_back({i, std::sqrt(d2)});
            }
        }
        std::sort
        (
            directNeighbours.begin(),
            directNeighbours.end(),
            []
            (
                const std::pair<uint64_t, float> & a,
                const std::pair<uint64_t, float> & b
            ) { return a.second == b.second ? a.first < b.first : a.second < b.second; }
        );
        return directNeighbours;
    }


private:

    glm::vec3 minPoint;
    glm::vec3 length;
    uint64_t nx, ny, nz;
    glm::vec3 domainSize;

    std::vector<std::vector<uint64_t>> domains;
    std::vector<uint64_t> domainAssignments;

    inline glm::vec<3, uint64_t, glm::packed_highp> domainCoords(const glm::vec3 & position) const
    {
        return glm::floor((position-minPoint)/domainSize);
    }

    inline uint64_t hash(const glm::vec3 & position) const
    {
        auto c = domainCoords(position);
        return c.x*ny*nz+c.y*nz+c.z;
    }
};

#endif /* NEIGHBOURS_H */
