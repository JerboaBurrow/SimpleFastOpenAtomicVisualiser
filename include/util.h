#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <string>

#include <glm/glm.hpp>

/**
 * @brief Map cartesian coordinates to spherical.
 *
 * @remark The axis are OpenGL form e.g.
 * OpenGL Axes.
 *       +y |   -z
 *          | /
 *          |/
 *   -x ----/--- +x
 *         /|
 *     +z / | -y
 * @remark The spherical coordinates are length,
 * polar angle (resp. +y), and azimuthal angle (resp. +x);
 * @param xyz cartesian vector.
 * @return glm::vec3 spherical vector.
 */
glm::vec3 cartesian2Spherical(glm::vec3 xyz)
{
    float r = glm::length(xyz);
    float sgn = (-1.0*xyz.z) < 0 ? -1.0 : 1.0;
    return
    {
        r,
        std::acos(xyz.y/r),
        sgn * std::acos(xyz.x/std::sqrt(xyz.x*xyz.x+xyz.z*xyz.z))
    };
}

/**
 * @brief convert spherical coordinates to cartesian coordinates.
 *
 * @remark The axis are OpenGL form e.g.
 * OpenGL Axes.
 *       +y |   -z
 *          | /
 *          |/
 *   -x ----/--- +x
 *         /|
 *     +z / | -y
 * @remark The spherical coordinates are length,
 * polar angle (resp. +y), and azimuthal angle (resp. +x);
 * @param rthetaphi the spherical vector.
 * @return glm::vec3 the cartesian vector.
 */
glm::vec3 spherical2cartesian(glm::vec3 rthetaphi)
{

    return
    {
        rthetaphi.x*std::sin(rthetaphi.y)*std::cos(rthetaphi.z),
        rthetaphi.x*std::cos(rthetaphi.y),
        -rthetaphi.x*std::sin(rthetaphi.y)*std::sin(rthetaphi.z)
    };
}

/**
 * @brief Subtract the centre of mass of some Atoms
 *
 * @param atoms the Atom list to centre.
 */
void center(std::vector<Atom> & atoms)
{
    glm::vec3 com = glm::vec3(0);
    for (const auto & atom : atoms)
    {
        com += atom.position;
    }
    com /= float(atoms.size());
    for (auto & atom : atoms)
    {
        atom.position -= com;
    }
}

/**
 * @brief Convert a double to a fixes length std::string.
 *
 * @param x the value to convert.
 * @param length the fixed lenght.
 * @return std::string the fixes length string representation of x.
 */
std::string fixedLengthNumber(double x, unsigned length)
{
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
}

#endif /* UTIL_H */
