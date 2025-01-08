#ifndef ATOM
#define ATOM

#include <cstdint>

#include <glm/glm.hpp>

class Atom
{
public:

    Atom
    (
        glm::vec3 position = glm::vec3(0),
        float scale = 1.0f,
        glm::vec3 velocity = glm::vec3(0),
        glm::vec3 force = glm::vec3(0),
        glm::vec4 colour = glm::vec4(1.0, 0.5, 0.5, 1.0)
    )
    : 
      position(position),
      velocity(velocity),
      force(force),
      colour(colour),
      scale(scale)
    {}

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    glm::vec4 colour;
    float scale;

private:
};

#endif /* ATOM */
