#ifndef ATOM_H
#define ATOM_H

#include <cstdint>
#include <iostream>

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
        glm::vec3 velocity = glm::vec3(0),
        glm::vec3 force = glm::vec3(0),
        glm::vec4 colour = glm::vec4(1.0, 0.5, 0.5, 1.0)
    )
    :
      symbol(symbol),
      position(position),
      velocity(velocity),
      force(force),
      colour(colour),
      scale(scale)
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
        glm::vec3 velocity = glm::vec3(0),
        glm::vec3 force = glm::vec3(0),
        glm::vec4 colour = glm::vec4(1.0, 0.5, 0.5, 1.0)
    ) : Atom(
          Element::Unknown,
          position,
          scale,
          velocity,
          force,
          colour)
    {}

    Element symbol;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    glm::vec4 colour;
    float scale;

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

#endif /* ATOM_H */
