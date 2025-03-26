#ifndef ELEMENT_H
#define ELEMENT_H

#include <cstdint>
#include <map>
#include <iostream>

/**
 * @brief Representable elements
 * @remark Unknown is a catch all default.
 */
enum class Element : uint8_t
{
    Unknown,
    H,
    He,
    Li,
    Be,
    B,
    C,
    N,
    O,
    F,
    Ne,
    Na,
    Mg,
    Al,
    Si,
    P,
    S,
    Cl,
    Ar,
    K,
    Ca,
    Sc,
    Ti,
    V,
    Cr,
    Mn,
    Fe,
    Co,
    Ni,
    Cu,
    Zn,
    Ga,
    Ge,
    As,
    Se,
    Br,
    Kr,
    Rb,
    Sr,
    Y,
    Zr,
    Nb,
    Mo,
    Tc,
    Ru,
    Rh,
    Pd,
    Ag,
    Cd,
    In,
    Sn,
    Sb,
    Te,
    I,
    Xe,
    Cs,
    Ba,
    La,
    Ce,
    Pr,
    Nd,
    Pm,
    Sm,
    Eu,
    Gd,
    Tb,
    Dy,
    Ho,
    Er,
    Tm,
    Yb,
    Lu,
    Hf,
    Ta,
    W,
    Re,
    Os,
    Ir,
    Pt,
    Au,
    Hg,
    Tl,
    Pb,
    Bi,
    Po,
    At,
    Rn,
    Fr,
    Ra,
    Ac,
    Th,
    Pa,
    U,
    Np,
    Pu,
    Am,
    Cm,
    Bk,
    Cf,
    Es,
    Fm,
    Md,
    No,
    Lw
};

/**
 * @brief Map string symbols to Element.
 *
 */
const std::map<std::string, Element> ELEMENT_FROM_STRING =
{
    {"Unknown", Element::Unknown},
    {"H", Element::H},
    {"He", Element::He},
    {"Li", Element::Li},
    {"Be", Element::Be},
    {"B", Element::B},
    {"C", Element::C},
    {"N", Element::N},
    {"O", Element::O},
    {"F", Element::F},
    {"Ne", Element::Ne},
    {"Na", Element::Na},
    {"Mg", Element::Mg},
    {"Al", Element::Al},
    {"Si", Element::Si},
    {"P", Element::P},
    {"S", Element::S},
    {"Cl", Element::Cl},
    {"Ar", Element::Ar},
    {"K", Element::K},
    {"Ca", Element::Ca},
    {"Sc", Element::Sc},
    {"Ti", Element::Ti},
    {"V", Element::V},
    {"Cr", Element::Cr},
    {"Mn", Element::Mn},
    {"Fe", Element::Fe},
    {"Co", Element::Co},
    {"Ni", Element::Ni},
    {"Cu", Element::Cu},
    {"Zn", Element::Zn},
    {"Ga", Element::Ga},
    {"Ge", Element::Ge},
    {"As", Element::As},
    {"Se", Element::Se},
    {"Br", Element::Br},
    {"Kr", Element::Kr},
    {"Rb", Element::Rb},
    {"Sr", Element::Sr},
    {"Y", Element::Y},
    {"Zr", Element::Zr},
    {"Nb", Element::Nb},
    {"Mo", Element::Mo},
    {"Tc", Element::Tc},
    {"Ru", Element::Ru},
    {"Rh", Element::Rh},
    {"Pd", Element::Pd},
    {"Ag", Element::Ag},
    {"Cd", Element::Cd},
    {"In", Element::In},
    {"Sn", Element::Sn},
    {"Sb", Element::Sb},
    {"Te", Element::Te},
    {"I", Element::I},
    {"Xe", Element::Xe},
    {"Cs", Element::Cs},
    {"Ba", Element::Ba},
    {"La", Element::La},
    {"Ce", Element::Ce},
    {"Pr", Element::Pr},
    {"Nd", Element::Nd},
    {"Pm", Element::Pm},
    {"Sm", Element::Sm},
    {"Eu", Element::Eu},
    {"Gd", Element::Gd},
    {"Tb", Element::Tb},
    {"Dy", Element::Dy},
    {"Ho", Element::Ho},
    {"Er", Element::Er},
    {"Tm", Element::Tm},
    {"Yb", Element::Yb},
    {"Lu", Element::Lu},
    {"Hf", Element::Hf},
    {"Ta", Element::Ta},
    {"W", Element::W},
    {"Re", Element::Re},
    {"Os", Element::Os},
    {"Ir", Element::Ir},
    {"Pt", Element::Pt},
    {"Au", Element::Au},
    {"Hg", Element::Hg},
    {"Tl", Element::Tl},
    {"Pb", Element::Pb},
    {"Bi", Element::Bi},
    {"Po", Element::Po},
    {"At", Element::At},
    {"Rn", Element::Rn},
    {"Fr", Element::Fr},
    {"Ra", Element::Ra},
    {"Ac", Element::Ac},
    {"Th", Element::Th},
    {"Pa", Element::Pa},
    {"U", Element::U},
    {"Np", Element::Np},
    {"Pu", Element::Pu},
    {"Am", Element::Am},
    {"Cm", Element::Cm},
    {"Bk", Element::Bk},
    {"Cf", Element::Cf},
    {"Es", Element::Es},
    {"Fm", Element::Fm},
    {"Md", Element::Md},
    {"No", Element::No},
    {"Lw", Element::Lw}
};

/**
 * @brief Map Element names to string symbols.
 *
 */
const std::map<std::string, std::string> ELEMENT_NAME_TO_STRING_SYMBOL =
{
    {"unknown", "Unknown"},
    {"hydrogen", "H"},
    {"helium", "He"},
    {"lithium", "Li"},
    {"beryllium", "Be"},
    {"boron", "B"},
    {"carbon", "C"},
    {"nitrogen", "N"},
    {"oxygen", "O"},
    {"fluorine", "F"},
    {"neon", "Ne"},
    {"sodium", "Na"},
    {"magnesium", "Mg"},
    {"aluminium", "Al"},
    {"silicon", "Si"},
    {"phosphorus", "P"},
    {"sulfur", "S"},
    {"chlorine", "Cl"},
    {"argon", "Ar"},
    {"potassium", "K"},
    {"calcium", "Ca"},
    {"scandium", "Sc"},
    {"titanium", "Ti"},
    {"vanadium", "V"},
    {"chromium", "Cr"},
    {"manganese", "Mn"},
    {"iron", "Fe"},
    {"cobalt", "Co"},
    {"nickel", "Ni"},
    {"copper", "Cu"},
    {"zinc", "Zn"},
    {"gallium", "Ga"},
    {"germanium", "Ge"},
    {"arsenic", "As"},
    {"selenium", "Se"},
    {"bromine", "Br"},
    {"krypton", "Kr"},
    {"rubidium", "Rb"},
    {"strontium", "Sr"},
    {"yttrium", "Y"},
    {"zirconium", "Zr"},
    {"niobium", "Nb"},
    {"molybdenum", "Mo"},
    {"technetium", "Tc"},
    {"ruthenium", "Ru"},
    {"rhodium", "Rh"},
    {"palladium", "Pd"},
    {"silver", "Ag"},
    {"cadmium", "Cd"},
    {"indium", "In"},
    {"tin", "Sn"},
    {"antimony", "Sb"},
    {"tellurium", "Te"},
    {"iodine", "I"},
    {"xenon", "Xe"},
    {"caesium", "Cs"},
    {"barium", "Ba"},
    {"lanthanum", "La"},
    {"cerium", "Ce"},
    {"praseodymium", "Pr"},
    {"neodymium", "Nd"},
    {"promethium", "Pm"},
    {"samarium", "Sm"},
    {"europium", "Eu"},
    {"gadolinium", "Gd"},
    {"terbium", "Tb"},
    {"dysprosium", "Dy"},
    {"holmium", "Ho"},
    {"erbium", "Er"},
    {"thulium", "Tm"},
    {"ytterbium", "Yb"},
    {"lutetium", "Lu"},
    {"hafnium", "Hf"},
    {"tantalum", "Ta"},
    {"tungsten", "W"},
    {"rhenium", "Re"},
    {"osmium", "Os"},
    {"iridium", "Ir"},
    {"platinum", "Pt"},
    {"gold", "Au"},
    {"mercury", "Hg"},
    {"thallium", "Tl"},
    {"lead", "Pb"},
    {"bismuth", "Bi"},
    {"polonium", "Po"},
    {"astatine", "At"},
    {"radon", "Rn"},
    {"francium", "Fr"},
    {"radium", "Ra"},
    {"actinium", "Ac"},
    {"thorium", "Th"},
    {"protactinium", "Pa"},
    {"uranium", "U"},
    {"neptunium", "Np"},
    {"plutonium", "Pu"},
    {"americium", "Am"},
    {"curium", "Cm"},
    {"berkelium", "Bk"},
    {"californium", "Cf"},
    {"einsteinium", "Es"},
    {"fermium", "Fm"},
    {"mendelevium", "Md"},
    {"nobelium", "No"},
    {"lawrencium", "Lw"}
};

/**
 * @brief Map Element to string symbols.
 *
 */
const std::map<Element, std::string> STRING_FROM_ELEMENT =
{
    {Element::Unknown, "Unknown"},
    {Element::H, "H"},
    {Element::He, "He"},
    {Element::Li, "Li"},
    {Element::Be, "Be"},
    {Element::B, "B"},
    {Element::C, "C"},
    {Element::N, "N"},
    {Element::O, "O"},
    {Element::F, "F"},
    {Element::Ne, "Ne"},
    {Element::Na, "Na"},
    {Element::Mg, "Mg"},
    {Element::Al, "Al"},
    {Element::Si, "Si"},
    {Element::P, "P"},
    {Element::S, "S"},
    {Element::Cl, "Cl"},
    {Element::Ar, "Ar"},
    {Element::K, "K"},
    {Element::Ca, "Ca"},
    {Element::Sc, "Sc"},
    {Element::Ti, "Ti"},
    {Element::V, "V"},
    {Element::Cr, "Cr"},
    {Element::Mn, "Mn"},
    {Element::Fe, "Fe"},
    {Element::Co, "Co"},
    {Element::Ni, "Ni"},
    {Element::Cu, "Cu"},
    {Element::Zn, "Zn"},
    {Element::Ga, "Ga"},
    {Element::Ge, "Ge"},
    {Element::As, "As"},
    {Element::Se, "Se"},
    {Element::Br, "Br"},
    {Element::Kr, "Kr"},
    {Element::Rb, "Rb"},
    {Element::Sr, "Sr"},
    {Element::Y, "Y"},
    {Element::Zr, "Zr"},
    {Element::Nb, "Nb"},
    {Element::Mo, "Mo"},
    {Element::Tc, "Tc"},
    {Element::Ru, "Ru"},
    {Element::Rh, "Rh"},
    {Element::Pd, "Pd"},
    {Element::Ag, "Ag"},
    {Element::Cd, "Cd"},
    {Element::In, "In"},
    {Element::Sn, "Sn"},
    {Element::Sb, "Sb"},
    {Element::Te, "Te"},
    {Element::I, "I"},
    {Element::Xe, "Xe"},
    {Element::Cs, "Cs"},
    {Element::Ba, "Ba"},
    {Element::La, "La"},
    {Element::Ce, "Ce"},
    {Element::Pr, "Pr"},
    {Element::Nd, "Nd"},
    {Element::Pm, "Pm"},
    {Element::Sm, "Sm"},
    {Element::Eu, "Eu"},
    {Element::Gd, "Gd"},
    {Element::Tb, "Tb"},
    {Element::Dy, "Dy"},
    {Element::Ho, "Ho"},
    {Element::Er, "Er"},
    {Element::Tm, "Tm"},
    {Element::Yb, "Yb"},
    {Element::Lu, "Lu"},
    {Element::Hf, "Hf"},
    {Element::Ta, "Ta"},
    {Element::W, "W"},
    {Element::Re, "Re"},
    {Element::Os, "Os"},
    {Element::Ir, "Ir"},
    {Element::Pt, "Pt"},
    {Element::Au, "Au"},
    {Element::Hg, "Hg"},
    {Element::Tl, "Tl"},
    {Element::Pb, "Pb"},
    {Element::Bi, "Bi"},
    {Element::Po, "Po"},
    {Element::At, "At"},
    {Element::Rn, "Rn"},
    {Element::Fr, "Fr"},
    {Element::Ra, "Ra"},
    {Element::Ac, "Ac"},
    {Element::Th, "Th"},
    {Element::Pa, "Pa"},
    {Element::U, "U"},
    {Element::Np, "Np"},
    {Element::Pu, "Pu"},
    {Element::Am, "Am"},
    {Element::Cm, "Cm"},
    {Element::Bk, "Bk"},
    {Element::Cf, "Cf"},
    {Element::Es, "Es"},
    {Element::Fm, "Fm"},
    {Element::Md, "Md"},
    {Element::No, "No"},
    {Element::Lw, "Lw"}
};

/**
 * @brief Map Element to a Van der Waals radius in Angstroms.
 * @remark Default is 1.0
 * @remark Data taken from https://doi.org/10.1039/C3DT50599E.
 */
const std::map<Element, float> ELEMENT_RADIUS =
{
    {Element::Unknown, 1.0},
    {Element::H, 1.2},
    {Element::He, 1.43},
    {Element::Li, 2.12},
    {Element::Be, 1.98},
    {Element::B, 1.91},
    {Element::C, 1.77},
    {Element::N, 1.66},
    {Element::O, 1.5},
    {Element::F, 1.46},
    {Element::Ne, 1.58},
    {Element::Na, 2.5},
    {Element::Mg, 2.51},
    {Element::Al, 2.25},
    {Element::Si, 2.19},
    {Element::P, 1.9},
    {Element::S, 1.89},
    {Element::Cl, 1.82},
    {Element::Ar, 1.83},
    {Element::K, 2.73},
    {Element::Ca, 2.62},
    {Element::Sc, 2.58},
    {Element::Ti, 2.46},
    {Element::V, 2.42},
    {Element::Cr, 2.45},
    {Element::Mn, 2.45},
    {Element::Fe, 2.44},
    {Element::Co, 2.4},
    {Element::Ni, 2.4},
    {Element::Cu, 2.38},
    {Element::Zn, 2.39},
    {Element::Ga, 2.32},
    {Element::Ge, 50.0},
    {Element::As, 1.88},
    {Element::Se, 1.82},
    {Element::Br, 1.86},
    {Element::Kr, 2.25},
    {Element::Rb, 3.21},
    {Element::Sr, 2.84},
    {Element::Y, 2.75},
    {Element::Zr, 2.52},
    {Element::Nb, 2.56},
    {Element::Mo, 2.45},
    {Element::Tc, 2.44},
    {Element::Ru, 2.46},
    {Element::Rh, 2.44},
    {Element::Pd, 2.15},
    {Element::Ag, 2.53},
    {Element::Cd, 2.49},
    {Element::In, 2.43},
    {Element::Sn, 2.42},
    {Element::Sb, 2.47},
    {Element::Te, 1.99},
    {Element::I, 2.04},
    {Element::Xe, 2.06},
    {Element::Cs, 3.48},
    {Element::Ba, 3.03},
    {Element::La, 2.98},
    {Element::Ce, 2.88},
    {Element::Pr, 2.92},
    {Element::Nd, 2.95},
    {Element::Sm, 2.9},
    {Element::Eu, 2.87},
    {Element::Gd, 2.83},
    {Element::Tb, 2.79},
    {Element::Dy, 2.87},
    {Element::Ho, 2.81},
    {Element::Er, 2.83},
    {Element::Tm, 2.79},
    {Element::Yb, 2.8},
    {Element::Lu, 2.74},
    {Element::Hf, 2.63},
    {Element::Ta, 2.53},
    {Element::W, 2.57},
    {Element::Re, 2.49},
    {Element::Os, 2.48},
    {Element::Ir, 2.41},
    {Element::Pt, 2.29},
    {Element::Au, 2.32},
    {Element::Hg, 2.45},
    {Element::Tl, 2.47},
    {Element::Pb, 2.6},
    {Element::Bi, 2.54},
    {Element::Ac, 2.8},
    {Element::Th, 2.93},
    {Element::Pa, 2.88},
    {Element::U, 2.71},
    {Element::Np, 2.82},
    {Element::Pu, 2.81},
    {Element::Am, 2.83},
    {Element::Cm, 3.05},
    {Element::Bk, 3.4},
    {Element::Cf, 3.05},
    {Element::Es, 2.7},
};

/**
 * @brief Scaled element masses
 * @remark Scaled as log10(m)+(1.0-log10(mH)) for element mass m
 * and mass of hydrogen mH both in AMU.
 */
const std::map<Element, float> ELEMENT_MASS =
{
    {Element::Unknown, 1.0},
    {Element::H, 1.0},
    {Element::He, 1.599},
    {Element::Li, 1.842},
    {Element::Be, 1.951},
    {Element::B, 2.03},
    {Element::C, 2.076},
    {Element::N, 2.143},
    {Element::O, 2.201},
    {Element::F, 2.275},
    {Element::Ne, 2.301},
    {Element::Na, 2.358},
    {Element::Mg, 2.382},
    {Element::Al, 2.428},
    {Element::Si, 2.445},
    {Element::P, 2.488},
    {Element::S, 2.503},
    {Element::Cl, 2.546},
    {Element::Ar, 2.598},
    {Element::K, 2.589},
    {Element::Ca, 2.599},
    {Element::Sc, 2.649},
    {Element::Ti, 2.677},
    {Element::V, 2.704},
    {Element::Cr, 2.713},
    {Element::Mn, 2.736},
    {Element::Fe, 2.743},
    {Element::Co, 2.767},
    {Element::Ni, 2.765},
    {Element::Cu, 2.8},
    {Element::Zn, 2.812},
    {Element::Ga, 2.84},
    {Element::Ge, 2.858},
    {Element::As, 2.871},
    {Element::Se, 2.894},
    {Element::Br, 2.899},
    {Element::Kr, 2.92},
    {Element::Rb, 2.928},
    {Element::Sr, 2.939},
    {Element::Y, 2.945},
    {Element::Zr, 2.957},
    {Element::Nb, 2.965},
    {Element::Mo, 2.979},
    {Element::Tc, 2.983},
    {Element::Ru, 3.001},
    {Element::Rh, 3.009},
    {Element::Pd, 3.024},
    {Element::Ag, 3.029},
    {Element::Cd, 3.047},
    {Element::In, 3.057},
    {Element::Sn, 3.071},
    {Element::Sb, 3.082},
    {Element::Te, 3.102},
    {Element::I, 3.1},
    {Element::Xe, 3.115},
    {Element::Cs, 3.12},
    {Element::Ba, 3.134},
    {Element::La, 3.139},
    {Element::Ce, 3.143},
    {Element::Pr, 3.145},
    {Element::Nd, 3.156},
    {Element::Pm, 3.158},
    {Element::Sm, 3.174},
    {Element::Eu, 3.178},
    {Element::Gd, 3.193},
    {Element::Tb, 3.198},
    {Element::Dy, 3.207},
    {Element::Ho, 3.214},
    {Element::Er, 3.22},
    {Element::Tm, 3.224},
    {Element::Yb, 3.235},
    {Element::Lu, 3.239},
    {Element::Hf, 3.248},
    {Element::Ta, 3.254},
    {Element::W, 3.261},
    {Element::Re, 3.267},
    {Element::Os, 3.276},
    {Element::Ir, 3.28},
    {Element::Pt, 3.287},
    {Element::Au, 3.291},
    {Element::Hg, 3.299},
    {Element::Tl, 3.307},
    {Element::Pb, 3.313},
    {Element::Bi, 3.317},
    {Element::Po, 3.317},
    {Element::At, 3.319},
    {Element::Rn, 3.343},
    {Element::Fr, 3.345},
    {Element::Ra, 3.351},
    {Element::Ac, 3.353},
    {Element::Th, 3.362},
    {Element::Pa, 3.36},
    {Element::U, 3.373},
    {Element::Np, 3.371},
    {Element::Pu, 3.384},
    {Element::Am, 3.382},
    {Element::Cm, 3.389},
    {Element::Bk, 3.389},
    {Element::Cf, 3.396},
    {Element::Es, 3.398},
    {Element::Fm, 3.407},
    {Element::Md, 3.408},
    {Element::No, 3.41}
};

/**
 * @brief Map a string symbol to an Element.
 *
 * @remark Defaults to Element::Unknown.
 * @remark If s is not an element symbol (C, Pt, Au, ...) attempts to
 * match with element names (carbon, platinum, gold, ...).
 * If all else fails the first two letters then first letter will be checked for a match.
 * @param s the string symbol
 * @return Element the element.
 */
Element stringSymbolToElement(const std::string & s)
{
    std::string stripped = s;
    stripped.erase(std::remove_if(stripped.begin(), stripped.end(), [](const char & c) { return !std::isalpha(c); }), stripped.end());
    if (stripped.size() < s.size()) { return stringSymbolToElement(stripped); }
    if (ELEMENT_FROM_STRING.find(s) != ELEMENT_FROM_STRING.cend())
    {
        return ELEMENT_FROM_STRING.at(s);
    }
    if (s.size() == 1)
    {
        std::string s1 = s.substr(0, 1);
        s1[0] = std::toupper(s1[0]);
        if (ELEMENT_FROM_STRING.find(s1) != ELEMENT_FROM_STRING.cend())
        {
            return ELEMENT_FROM_STRING.at(s1);
        }
    }
    else if (s.size() == 2)
    {
        std::string s2 = s.substr(0, 2);
        s2[0] = std::toupper(s2[0]);
        s2[1] = std::tolower(s2[1]);
        if (ELEMENT_FROM_STRING.find(s2) != ELEMENT_FROM_STRING.cend())
        {
            return ELEMENT_FROM_STRING.at(s2);
        }
        return stringSymbolToElement(s.substr(0, 1));
    }
    else if (s.size() > 2)
    {
        std::string name = s;
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
        if (ELEMENT_NAME_TO_STRING_SYMBOL.find(name) != ELEMENT_NAME_TO_STRING_SYMBOL.cend())
        {
            return ELEMENT_FROM_STRING.at(ELEMENT_NAME_TO_STRING_SYMBOL.at(name));
        }
        std::vector<std::string> names;
        for (const auto & ns : ELEMENT_NAME_TO_STRING_SYMBOL)
        {
            names.push_back(ns.first);
        }
        uint64_t i = 1;
        while (names.size() > 1)
        {
            std::string check = name.substr(0, i);
            std::vector<std::string> newNames;
            for (const auto & n : names)
            {
                if (n.rfind(check, 0) == 0)
                {
                    newNames.push_back(n);
                }
            }
            i++;
            names = newNames;
            if (i > name.size()) { break; }
        }
        if (names.size() == 1) { return ELEMENT_FROM_STRING.at(ELEMENT_NAME_TO_STRING_SYMBOL.at(names[0])); }
        return stringSymbolToElement(s.substr(0, 2));
    }
    return Element::Unknown;
}

/**
 * @brief Map a string symbol to a VDW radius.
 *
 * @param s the string symbol to map.
 * @return float the radius.
 */
float stringSymbolToElementRadius(const std::string & s)
{
    Element element = stringSymbolToElement(s);
    if (ELEMENT_RADIUS.find(element) == ELEMENT_RADIUS.cend())
    {
        return 1.0;
    }
    return ELEMENT_RADIUS.at(element);
}

/**
 * @brief Print an Element to std::ostream.
 *
 * @remark Prints the result of @see stringSymbolToElement.
 * @param o the output stream to print to.
 * @param e the Element to print.
 * @return std::ostream& the modified output stream.
 */
std::ostream & operator <<(std::ostream & o, const Element & e)
{
    return o << STRING_FROM_ELEMENT.at(e);
}

#endif /* ELEMENT_H */
