#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <string>
#include <limits.h>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <regex>

#include <glm/glm.hpp>

#include <colour.h>

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

std::vector<std::string> split(std::string str, std::regex delim)
{
    std::vector<std::string> s;
    auto matches = std::sregex_iterator(str.begin(), str.end(), delim);
    if (std::distance(matches, std::sregex_iterator()) == 0) { return {str}; }
    auto key = matches->position();
    if (key > 0)
    {
        s.push_back(str.substr(0, key-1));
    }
    while (matches != std::sregex_iterator())
    {
        key = matches->position();
        matches++;
        auto next = matches->position();
        s.push_back(str.substr(key, next-1-key));
    }
    return s;
}

const std::vector<Atom> sfoavAtoms =
{
    {Element::S, {-11.3966, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-10.3966, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-9.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-10.3966, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-9.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-8.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-8.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-11.3966, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-11.3966, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-10.3966, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-9.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::S, {-8.39655, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::S), CPK_COLOURS.at(Element::S)},
    {Element::F, {-6.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-5.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-4.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-6.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-5.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-6.39655, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-6.39655, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-6.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-4.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::F, {-3.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::F), CPK_COLOURS.at(Element::F)},
    {Element::Ar, {-1.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-0.396552, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {0.603448, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {0.603448, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-0.396552, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::O, {3.60345, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {3.60345, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {3.60345, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {3.60345, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {3.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {4.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {5.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {6.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {6.60345, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {6.60345, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {6.60345, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {6.60345, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {5.60345, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {4.60345, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::V, {8.60345, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {11.6034, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {8.60345, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {8.60345, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {11.6034, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {11.6034, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {10.6034, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {9.60345, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {8.60345, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)},
    {Element::V, {11.6034, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::V), CPK_COLOURS.at(Element::V)}
};

#endif /* UTIL_H */
