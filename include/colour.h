#ifndef COLOUR_H
#define COLOUR_H

#include <map>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>

#include <element.h>

/**
 * @brief Corey–Pauling–Koltun colourings.
 *
 */
const std::map<Element, glm::vec4> CPK_COLOURS =
{
    { Element::Unknown, {1.0,0.5,0.5,1.0}},
    { Element::H, {1.0,1.0,1.0,1.0}},
    { Element::He, {1.0,0.753,0.796,1.0}},
    { Element::Li, {0.698,0.133,0.133,1.0}},
    { Element::Be, {1.0,0.078,0.576,1.0}},
    { Element::B, {0.0,1.0,0.0,1.0}},
    { Element::C, {0.784,0.784,0.784,1.0}},
    { Element::N, {0.561,0.561,1.0,1.0}},
    { Element::O, {0.941,0.0,0.0,1.0}},
    { Element::F, {0.855,0.647,0.125,1.0}},
    { Element::Ne, {1.0,0.078,0.576,1.0}},
    { Element::Na, {0.0,0.0,1.0,1.0}},
    { Element::Mg, {0.133,0.545,0.133,1.0}},
    { Element::Al, {0.502,0.502,0.565,1.0}},
    { Element::Si, {0.855,0.647,0.125,1.0}},
    { Element::P, {1.0,0.647,0.0,1.0}},
    { Element::S, {1.0,0.784,0.196,1.0}},
    { Element::Cl, {0.0,1.0,0.0,1.0}},
    { Element::Ar, {1.0,0.078,0.576,1.0}},
    { Element::K, {1.0,0.078,0.576,1.0}},
    { Element::Ca, {0.502,0.502,0.565,1.0}},
    { Element::Sc, {1.0,0.078,0.576,1.0}},
    { Element::Ti, {0.502,0.502,0.565,1.0}},
    { Element::V, {1.0,0.078,0.576,1.0}},
    { Element::Cr, {0.502,0.502,0.565,1.0}},
    { Element::Mn, {0.502,0.502,0.565,1.0}},
    { Element::Fe, {1.0,0.647,0.0,1.0}},
    { Element::Co, {1.0,0.078,0.576,1.0}},
    { Element::Ni, {0.647,0.165,0.165,1.0}},
    { Element::Cu, {0.647,0.165,0.165,1.0}},
    { Element::Zn, {0.647,0.165,0.165,1.0}},
    { Element::Ga, {1.0,0.078,0.576,1.0}},
    { Element::Ge, {1.0,0.078,0.576,1.0}},
    { Element::As, {1.0,0.078,0.576,1.0}},
    { Element::Se, {1.0,0.078,0.576,1.0}},
    { Element::Br, {0.647,0.165,0.165,1.0}},
    { Element::Kr, {1.0,0.078,0.576,1.0}},
    { Element::Rb, {1.0,0.078,0.576,1.0}},
    { Element::Sr, {1.0,0.078,0.576,1.0}},
    { Element::Y, {1.0,0.078,0.576,1.0}},
    { Element::Zr, {1.0,0.078,0.576,1.0}},
    { Element::Nb, {1.0,0.078,0.576,1.0}},
    { Element::Mo, {1.0,0.078,0.576,1.0}},
    { Element::Tc, {1.0,0.078,0.576,1.0}},
    { Element::Ru, {1.0,0.078,0.576,1.0}},
    { Element::Rh, {1.0,0.078,0.576,1.0}},
    { Element::Pd, {1.0,0.078,0.576,1.0}},
    { Element::Ag, {0.502,0.502,0.565,1.0}},
    { Element::Cd, {1.0,0.078,0.576,1.0}},
    { Element::In, {1.0,0.078,0.576,1.0}},
    { Element::Sn, {1.0,0.078,0.576,1.0}},
    { Element::Sb, {1.0,0.078,0.576,1.0}},
    { Element::Te, {1.0,0.078,0.576,1.0}},
    { Element::I, {0.627,0.125,0.941,1.0}},
    { Element::Xe, {1.0,0.078,0.576,1.0}},
    { Element::Cs, {1.0,0.078,0.576,1.0}},
    { Element::Ba, {1.0,0.647,0.0,1.0}},
    { Element::La, {1.0,0.078,0.576,1.0}},
    { Element::Ce, {1.0,0.078,0.576,1.0}},
    { Element::Pr, {1.0,0.078,0.576,1.0}},
    { Element::Nd, {1.0,0.078,0.576,1.0}},
    { Element::Pm, {1.0,0.078,0.576,1.0}},
    { Element::Sm, {1.0,0.078,0.576,1.0}},
    { Element::Eu, {1.0,0.078,0.576,1.0}},
    { Element::Gd, {1.0,0.078,0.576,1.0}},
    { Element::Tb, {1.0,0.078,0.576,1.0}},
    { Element::Dy, {1.0,0.078,0.576,1.0}},
    { Element::Ho, {1.0,0.078,0.576,1.0}},
    { Element::Er, {1.0,0.078,0.576,1.0}},
    { Element::Tm, {1.0,0.078,0.576,1.0}},
    { Element::Yb, {1.0,0.078,0.576,1.0}},
    { Element::Lu, {1.0,0.078,0.576,1.0}},
    { Element::Hf, {1.0,0.078,0.576,1.0}},
    { Element::Ta, {1.0,0.078,0.576,1.0}},
    { Element::W, {1.0,0.078,0.576,1.0}},
    { Element::Re, {1.0,0.078,0.576,1.0}},
    { Element::Os, {1.0,0.078,0.576,1.0}},
    { Element::Ir, {1.0,0.078,0.576,1.0}},
    { Element::Pt, {1.0,0.078,0.576,1.0}},
    { Element::Au, {0.855,0.647,0.125,1.0}},
    { Element::Hg, {1.0,0.078,0.576,1.0}},
    { Element::Tl, {1.0,0.078,0.576,1.0}},
    { Element::Pb, {1.0,0.078,0.576,1.0}},
    { Element::Bi, {1.0,0.078,0.576,1.0}},
    { Element::Po, {1.0,0.078,0.576,1.0}},
    { Element::At, {1.0,0.078,0.576,1.0}},
    { Element::Rn, {1.0,1.0,1.0,1.0}},
    { Element::Fr, {1.0,1.0,1.0,1.0}},
    { Element::Ra, {1.0,1.0,1.0,1.0}},
    { Element::Ac, {1.0,1.0,1.0,1.0}},
    { Element::Th, {1.0,0.078,0.576,1.0}},
    { Element::Pa, {1.0,1.0,1.0,1.0}},
    { Element::U, {1.0,0.078,0.576,1.0}},
    { Element::Np, {1.0,1.0,1.0,1.0}},
    { Element::Pu, {1.0,1.0,1.0,1.0}},
    { Element::Am, {1.0,1.0,1.0,1.0}},
    { Element::Cm, {1.0,1.0,1.0,1.0}},
    { Element::Bk, {1.0,1.0,1.0,1.0}},
    { Element::Cf, {1.0,1.0,1.0,1.0}},
    { Element::Es, {1.0,1.0,1.0,1.0}},
    { Element::Fm, {1.0,1.0,1.0,1.0}},
    { Element::Md, {1.0,1.0,1.0,1.0}},
    { Element::No, {1.0,1.0,1.0,1.0}},
    { Element::Lw, {1.0,1.0,1.0,1.0}}
};

/**
 * @brief Map an Element to a CPK colour.
 *
 * @param e the Element type.
 * @return glm::vec4 the colour.
 */
glm::vec4 elementToColour(Element & e)
{
    return CPK_COLOURS.at(e);
}

/**
 * @brief Map a string symbol to a CPK colour.
 *
 * @param s the element symbol.
 * @return glm::vec4 the colour.
 */
glm::vec4 stringSymbolToColour(std::string & s)
{
    return CPK_COLOURS.at(stringSymbolToElement(s));
}

/**
 * @brief Read an Element colour map from a file.
 *
 * @remark The file should be formatted with lines a the element name string and 4 floats.
 * @remark Any unspecified colourings will default to CPK.
 * @remark CPK colours are returned on errors.
 * @param path the file path.
 * @return std::map<Element, glm::vec4> the Element colourings.
 */
std::map<Element, glm::vec4> coloursFromFile(std::filesystem::path path)
{
    if (std::filesystem::exists(path))
    {
        std::map<Element, glm::vec4> colours(CPK_COLOURS);
        std::ifstream in(path);
        std::string line;
        std::stringstream ss;
        std::string name;
        Element element;
        float r, g, b, a;
        while (std::getline(in, line))
        {
            ss = std::stringstream(line);
            ss >> name >> r >> g >> b >> a;
            element = stringSymbolToElement(name);
            if (!ss.fail() && element != Element::Unknown)
            {
                colours[element] = glm::vec4(r, g, b, a);
            }
        }

        return colours;
    }
    else
    {
        std::cout << "Could not find colourmap file " << path << " defaulting to CPK\n";
        return CPK_COLOURS;
    }
}

#endif /* COLOUR_H */
