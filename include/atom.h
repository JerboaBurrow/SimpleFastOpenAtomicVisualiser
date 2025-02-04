#ifndef ATOM_H
#define ATOM_H

#include <cstdint>
#include <iostream>
#include <set>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <element.h>
#include <colour.h>

/**
 * @brief An atom structure.
 *
 */
class Atom
{
public:

    /**
     * @brief Construct a Atom of a given Element.
     *
     * @param Element the atoms element type.
     * @param position the atoms cartesian coordinates.
     * @param scale the atoms spherical radius.
     * @param velocity the atoms velocity.
     * @param force the atoms force.
     * @param colour the atoms colour.
     */
    Atom
    (
        Element symbol,
        glm::vec3 position = glm::vec3(0),
        float scale = 1.0f,
        glm::vec4 colour = glm::vec4(1.0, 0.5, 0.5, 1.0),
        glm::vec3 velocity = glm::vec3(0),
        glm::vec3 force = glm::vec3(0)
    )
    :
      symbol(symbol),
      position(position),
      scale(scale),
      colour(colour),
      velocity(velocity),
      force(force)
    {}

     /**
     * @brief Construct a Atom of unknown Element type.
     *
     * @param position the atoms cartesian coordinates.
     * @param scale the atoms spherical radius.
     * @param velocity the atoms velocity.
     * @param force the atoms force.
     * @param colour the atoms colour.
     */
    Atom
    (
        glm::vec3 position = glm::vec3(0),
        float scale = 1.0f,
        glm::vec4 colour = glm::vec4(1.0, 0.5, 0.5, 1.0),
        glm::vec3 velocity = glm::vec3(0),
        glm::vec3 force = glm::vec3(0)
    ) : Atom(
          Element::Unknown,
          position,
          scale,
          colour,
          velocity,
          force
        )
    {}

    Element symbol;
    glm::vec3 position;
    float scale;
    glm::vec4 colour;
    glm::vec3 velocity;
    glm::vec3 force;

private:
};

/**
 * @brief Print an atom to std::ostream.
 *
 * @remark Prints and atoms element symbol and position.
 * @param o output stream.
 * @param atom atom to print.
 * @return std::ostream& the resulting output stream.
 */
std::ostream & operator <<(std::ostream & o, Atom & atom)
{
    return o << atom.symbol << ": " << atom.position;
}

/**
 * @brief Calculate the centre of mass.
 *
 * @param atoms the Atom list to centre.
 * @return glm::vec3 the Atoms centers of mass.
 */
glm::vec3 getCenter(const std::vector<Atom> & atoms)
{
    glm::vec3 com = glm::vec3(0);
    for (const auto & atom : atoms)
    {
        com += atom.position;
    }
    return com / float(atoms.size());
}

/**
 * @brief Subtract the centre of mass of some Atoms
 *
 * @param atoms the Atom list to centre.
 */
void center(std::vector<Atom> & atoms)
{
    glm::vec3 com = getCenter(atoms);
    for (auto & atom : atoms)
    {
        atom.position -= com;
    }
}

void translate(std::vector<Atom> & atoms, glm::vec3 r)
{
    for (auto & atom : atoms) { atom.position += r; }
}

/**
 * @brief Calculate the extent of some Atoms
 *
 * @param atoms the Atom list.
 */
glm::vec3 extent(const std::vector<Atom> & atoms)
{
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());
    for (const auto & atom : atoms)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            min[i] = std::min(min[i], atom.position[i]);
            max[i] = std::max(max[i], atom.position[i]);
        }
    }
    return max-min;
}

/**
 * @brief Determine the unique elements in a list of Atom.
 *
 * @param atoms the atoms to check.
 * @return std::set<Element> the set of elements.
 */
std::set<Element> uniqueElements(const std::vector<Atom> & atoms)
{
    std::set<Element> e;
    for (const auto & atom : atoms)
    {
        e.insert(atom.symbol);
    }
    return e;
}

/**
 * @brief Obtain indices of each element.
 *
 * @param atoms the atoms to index.
 * @return std::multimap<Element, uint64_t> the indices of each element type.
 */
std::multimap<Element, uint64_t> elementIndices(const std::vector<Atom> & atoms)
{
    std::multimap<Element, uint64_t> m;
    for (uint64_t i = 0; i < atoms.size(); i++)
    {
        m.insert(std::pair(atoms[i].symbol, i));
    }
    return m;
}
#endif /* ATOM_H */
