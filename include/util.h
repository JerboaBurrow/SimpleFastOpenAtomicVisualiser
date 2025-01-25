#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <string>
#include <limits.h>
#include <filesystem>
#include <algorithm>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <xyz.h>
#include <config.h>

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

/**
 * @brief Read a structure file from the path.
 *
 * @remark Will attemp to automatically detect CONFIG-like of [EXT]XYZ files.
 * @remark Will try both on failure.
 * @param path the structure file's path.
 * @param structure the structure unique pointer.
 */
void readStructureFile(std::filesystem::path path, std::unique_ptr<Structure> & structure)
{
    if (!ostensiblyXYZLike(path))
    {
        if (!ostensiblyCONFIGLike(path))
        {
            std::cout << path << " does not appear to refer to an [EXT]XYZ or CONFIG-like\n";
        }
        try
        {
            structure = std::make_unique<CONFIG>(path);
        }
        catch (std::runtime_error & e)
        {
            std::cout << "Could not parse "
                      << path
                      << " as a CONFIG-like:\n"
                      << e.what() << "\n Trying [EXT]XYZ\n";
            structure = std::make_unique<XYZ>(path);
        }
    }
    else
    {
        try
        {
            structure = std::make_unique<XYZ>(path);
        }
        catch (std::runtime_error & e)
        {
            std::cout << "Could not parse "
                      << path
                      << " as an [EXT]XYZ:\n"
                      << e.what() << "\n Trying CONFIG-like\n";
            structure = std::make_unique<CONFIG>(path);
        }
    }
}

#endif /* UTIL_H */
