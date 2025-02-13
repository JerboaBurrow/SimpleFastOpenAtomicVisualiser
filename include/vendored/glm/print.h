#ifndef PRINT_H
#define PRINT_H

/*
    Simple additions to print vectors.

    MIT as the main repo (LICENSE.md).
*/

#include <iostream>
#include "vec2.hpp"
#include "vec3.hpp"

std::ostream & operator <<(std::ostream & o, glm::vec3 v)
{
    return o << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

std::ostream & operator <<(std::ostream & o, glm::vec2 v)
{
    return o << "[" << v.x << ", " << v.y << "]";
}

#endif /* PRINT_H */
