#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <string>
#include <sstream>
#include <limits.h>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <regex>

#include <GLFW/glfw3.h>

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

/**
 * @brief Split a std::string by a std::regex token.
 *
 * @param str the std::string to split.
 * @param delim the std::regex delimiter.
 * @return std::vector<std::string> the substrings split on delim.
 */
std::vector<std::string> split(std::string str, std::regex delim)
{
    std::vector<std::string> s;
    auto matches = std::sregex_iterator(str.begin(), str.end(), delim);
    if (std::distance(matches, std::sregex_iterator()) == 0) { return {str}; }
    auto key = matches->position();
    if (key > 0)
    {
        s.push_back(str.substr(0, key));
    }
    while (matches != std::sregex_iterator())
    {
        key = matches->position();
        matches++;
        auto next = matches->position();
        s.push_back(str.substr(key+1, next-1-key));
    }
    return s;
}

/**
 * @brief Combined two strings (split by newlines) side by side.
 *
 * @param left the strings to place on the left.
 * @param right the strings to place on the right.
 * @param rjust the start of the right hand strings.
 * @return std::string the combined strings.
 */
std::string sidebyside(std::string left, std::string right, uint8_t rjust)
{
    std::vector<std::string> l = split(left, std::regex("\\n"));
    std::vector<std::string> r = split(right, std::regex("\\n"));

    std::string combined;
    std::vector<std::string>::iterator riter = r.begin();
    for (auto liter = l.begin(); liter != l.end(); liter++)
    {
        combined += *liter;
        if (riter != r.end())
        {
            int padding = std::max(int(rjust)-int((*liter).size()), 0);
            combined += std::string(padding, ' ') + *riter + "\n";
            riter++;
        }
        else
        {
            combined += "\n";
        }
    }

    while (riter != r.end())
    {
        combined += std::string(rjust, ' ') + *riter + "\n";
        riter++;
    }
    return combined;
}

/**
 * @brief Current timestamp.
 * @remark In the format Thu-Mar-13-08-15-09-2025.
 * @return std::string the timestamp string.
 */
std::string timeStamp()
{
    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string ts = std::ctime(&end_time);
    std::replace(ts.begin(), ts.end(), ' ', '-');
    std::replace(ts.begin(), ts.end(), ':', '-');
    ts.erase(ts.end()-1);
    return ts;
}

/**
 * @brief A set of atoms spelling SFOAV to display during loading.
 *
 */
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
    {Element::O, {-1.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-1.39655, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-1.39655, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-1.39655, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-1.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-0.39655, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {0.603449, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {1.603449, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {1.603449, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {1.603449, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {1.603449, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {1.603449, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {0.603449, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::O, {-0.39655, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::O), CPK_COLOURS.at(Element::O)},
    {Element::Ar, {-1.39655+1.603449*3.0, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655+1.603449*3.0, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655+1.603449*3.0, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-1.39655+1.603449*3.0, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-0.396552+1.603449*3.0, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {0.603448+1.603449*3.0, 1.89655, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345+1.603449*3.0, 0.896552, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345+1.603449*3.0, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345+1.603449*3.0, -1.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {1.60345+1.603449*3.0, -2.10345, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {0.603448+1.603449*3.0, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
    {Element::Ar, {-0.396552+1.603449*3.0, -0.103448, 0.0}, 0.5f*ELEMENT_RADIUS.at(Element::Ar), CPK_COLOURS.at(Element::Ar)},
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

/**
 * @brief String name for a GLFW_KEY index.
 *
 */
const std::map<int, std::string> keyCodes
{
    {GLFW_KEY_UNKNOWN, "Unknown"},
    {GLFW_KEY_SPACE, "Space"},
    {GLFW_KEY_APOSTROPHE, "Apostrophe"},
    {GLFW_KEY_COMMA, "Comma"},
    {GLFW_KEY_MINUS, "Minus"},
    {GLFW_KEY_PERIOD, "Period"},
    {GLFW_KEY_SLASH, "Slash"},
    {GLFW_KEY_0, "0"},
    {GLFW_KEY_1, "1"},
    {GLFW_KEY_2, "2"},
    {GLFW_KEY_3, "3"},
    {GLFW_KEY_4, "4"},
    {GLFW_KEY_5, "5"},
    {GLFW_KEY_6, "6"},
    {GLFW_KEY_7, "7"},
    {GLFW_KEY_8, "8"},
    {GLFW_KEY_9, "9"},
    {GLFW_KEY_SEMICOLON, "Semicolon"},
    {GLFW_KEY_EQUAL, "Equal"},
    {GLFW_KEY_A, "A"},
    {GLFW_KEY_B, "B"},
    {GLFW_KEY_C, "C"},
    {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"},
    {GLFW_KEY_F, "F"},
    {GLFW_KEY_G, "G"},
    {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"},
    {GLFW_KEY_J, "J"},
    {GLFW_KEY_K, "K"},
    {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"},
    {GLFW_KEY_N, "N"},
    {GLFW_KEY_O, "O"},
    {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"},
    {GLFW_KEY_R, "R"},
    {GLFW_KEY_S, "S"},
    {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"},
    {GLFW_KEY_V, "V"},
    {GLFW_KEY_W, "W"},
    {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"},
    {GLFW_KEY_Z, "Z"},
    {GLFW_KEY_LEFT_BRACKET, "LeftBracket"},
    {GLFW_KEY_BACKSLASH, "Backslash"},
    {GLFW_KEY_RIGHT_BRACKET, "RightBracket"},
    {GLFW_KEY_GRAVE_ACCENT, "GraveAccent"},
    {GLFW_KEY_WORLD_1, "World1"},
    {GLFW_KEY_WORLD_2, "World2"},
    {GLFW_KEY_ESCAPE, "Escape"},
    {GLFW_KEY_ENTER, "Enter"},
    {GLFW_KEY_TAB, "Tab"},
    {GLFW_KEY_BACKSPACE, "Backspace"},
    {GLFW_KEY_INSERT, "Insert"},
    {GLFW_KEY_DELETE, "Delete"},
    {GLFW_KEY_RIGHT, "Right"},
    {GLFW_KEY_LEFT, "Left"},
    {GLFW_KEY_DOWN, "Down"},
    {GLFW_KEY_UP, "Up"},
    {GLFW_KEY_PAGE_UP, "PageUp"},
    {GLFW_KEY_PAGE_DOWN, "PageDown"},
    {GLFW_KEY_HOME, "Home"},
    {GLFW_KEY_END, "End"},
    {GLFW_KEY_CAPS_LOCK, "CapsLock"},
    {GLFW_KEY_SCROLL_LOCK, "ScrollLock"},
    {GLFW_KEY_NUM_LOCK, "NumLock"},
    {GLFW_KEY_PRINT_SCREEN, "PrintScreen"},
    {GLFW_KEY_PAUSE, "Pause"},
    {GLFW_KEY_F1, "F1"},
    {GLFW_KEY_F2, "F2"},
    {GLFW_KEY_F3, "F3"},
    {GLFW_KEY_F4, "F4"},
    {GLFW_KEY_F5, "F5"},
    {GLFW_KEY_F6, "F6"},
    {GLFW_KEY_F7, "F7"},
    {GLFW_KEY_F8, "F8"},
    {GLFW_KEY_F9, "F9"},
    {GLFW_KEY_F10, "F10"},
    {GLFW_KEY_F11, "F11"},
    {GLFW_KEY_F12, "F12"},
    {GLFW_KEY_F13, "F13"},
    {GLFW_KEY_F14, "F14"},
    {GLFW_KEY_F15, "F15"},
    {GLFW_KEY_F16, "F16"},
    {GLFW_KEY_F17, "F17"},
    {GLFW_KEY_F18, "F18"},
    {GLFW_KEY_F19, "F19"},
    {GLFW_KEY_F20, "F20"},
    {GLFW_KEY_F21, "F21"},
    {GLFW_KEY_F22, "F22"},
    {GLFW_KEY_F23, "F23"},
    {GLFW_KEY_F24, "F24"},
    {GLFW_KEY_F25, "F25"},
    {GLFW_KEY_KP_0, "Keypad0"},
    {GLFW_KEY_KP_1, "Keypad1"},
    {GLFW_KEY_KP_2, "Keypad2"},
    {GLFW_KEY_KP_3, "Keypad3"},
    {GLFW_KEY_KP_4, "Keypad4"},
    {GLFW_KEY_KP_5, "Keypad5"},
    {GLFW_KEY_KP_6, "Keypad6"},
    {GLFW_KEY_KP_7, "Keypad7"},
    {GLFW_KEY_KP_8, "Keypad8"},
    {GLFW_KEY_KP_9, "Keypad9"},
    {GLFW_KEY_KP_DECIMAL, "KeypadDecimal"},
    {GLFW_KEY_KP_DIVIDE, "KeypadDivide"},
    {GLFW_KEY_KP_MULTIPLY, "KeypadMultiply"},
    {GLFW_KEY_KP_SUBTRACT, "KeypadSubtract"},
    {GLFW_KEY_KP_ADD, "KeypadAdd"},
    {GLFW_KEY_KP_ENTER, "KeypadEnter"},
    {GLFW_KEY_KP_EQUAL, "KeypadEqual"},
    {GLFW_KEY_LEFT_SHIFT, "LeftShift"},
    {GLFW_KEY_LEFT_CONTROL, "LeftControl"},
    {GLFW_KEY_LEFT_ALT, "LeftAlt"},
    {GLFW_KEY_LEFT_SUPER, "LeftSuper"},
    {GLFW_KEY_RIGHT_SHIFT, "RightShift"},
    {GLFW_KEY_RIGHT_CONTROL, "RightControl"},
    {GLFW_KEY_RIGHT_ALT, "RightAlt"},
    {GLFW_KEY_RIGHT_SUPER, "RightSuper"},
    {GLFW_KEY_MENU, "Menu"}
};

#endif /* UTIL_H */
