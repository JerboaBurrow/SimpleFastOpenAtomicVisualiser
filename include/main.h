#ifndef MAIN
#define MAIN

#include <sstream>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <icon.h>

#include <jLog/jLog.h>

#include <HTM.h>

#include <atom.h>
#include <atomRenderer.h>
#include <chrono>

const unsigned int resX = 512;
const unsigned int resY = resX;

const float dr = (1.0)*0.5;
const float dtheta = (3.14)*0.025;
const float dphi = (2.0*3.14)*0.05;

std::unique_ptr<jGL::jGLInstance> jGLInstance;

std::ostream & operator <<(std::ostream & o, glm::vec3 v)
{
    return o << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

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

glm::vec3 spherical2cartesian(glm::vec3 rthetaphi)
{
    /* OpenGL Axes.
         +y |   -z
            | /
            |/
     -x ----/--- +x
           /|
       +z / | -y
    */
    return
    {
        rthetaphi.x*std::sin(rthetaphi.y)*std::cos(rthetaphi.z),
        rthetaphi.x*std::cos(rthetaphi.y),
        -rthetaphi.x*std::sin(rthetaphi.y)*std::sin(rthetaphi.z)
    };
}

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

#endif /* MAIN */