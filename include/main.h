#ifndef MAIN_H
#define MAIN_H

#include <sstream>
#include <chrono>
#include <exception>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <icon.h>

#include <jLog/jLog.h>

#include <atom.h>
#include <atomRenderer.h>
#include <bondRenderer.h>
#include <axes.h>
#include <util.h>
#include <glUtils.h>
#include <structureUtils.h>
#include <commandLine.h>
#include <xyz.h>
#include <config.h>
#include <camera.h>
#include <cell.h>

const float dr = (1.0)*0.5;
const float dtheta = (3.14)*0.025;
const float dphi = (2.0*3.14)*0.05;

const float emphasisedAlpha = 1.0f;

std::unique_ptr<jGL::jGLInstance> jGLInstance;

/**
 * @brief Controls for the camera
 *
 * @param display display to obtain events from.
 * @param camera the camera to update.
 */
void cameraControls(jGL::DesktopDisplay & display, Camera & camera)
{
    if (display.keyHasEvent(GLFW_KEY_W, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_W, jGL::EventType::HOLD))
    {
        camera.zoom(-dr);
    }
    if (display.keyHasEvent(GLFW_KEY_S, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_S, jGL::EventType::HOLD))
    {
        camera.zoom(dr);
    }
    if (display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::HOLD))
    {
        camera.incline(dtheta);
    }
    if (display.keyHasEvent(GLFW_KEY_E, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_E, jGL::EventType::HOLD))
    {
        camera.incline(-dtheta);
    }
    if (display.keyHasEvent(GLFW_KEY_A, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_A, jGL::EventType::HOLD))
    {
        camera.rotate(-dphi);
    }
    if (display.keyHasEvent(GLFW_KEY_D, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_D, jGL::EventType::HOLD))
    {
        camera.rotate(dphi);
    }
}

/**
 * @brief Controls for the Atoms position's
 *
 * @param display the display to get events from.
 * @param atoms the Atoms to modify.
 * @param emphasisControls key bindings to emphasise elements.
 *
 * @return true if the Atoms were modified.
 * @return false if the Atoms were not modified.
 */
bool atomControls
(
    jGL::DesktopDisplay & display,
    std::vector<Atom> & atoms,
    std::map<int, Element> & emphasisControls,
    std::multimap<Element, uint64_t> & elementMap,
    std::vector<float> & alphaOverrides,
    float deemphasisAlpha
)
{
    bool elementsNeedUpdate = false;
    if (display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::HOLD))
    {
        translate(atoms, {-dr, 0.0, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::HOLD))
    {
        translate(atoms, {dr, 0.0, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, -dr, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, dr, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, 0.0, -dr});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, 0.0, dr});
        elementsNeedUpdate = true;
    }

    for (const auto & control : emphasisControls)
    {
        if (display.keyHasEvent(control.first, jGL::EventType::PRESS))
        {
            auto iter = elementMap.equal_range(control.second);
            while (iter.first != iter.second)
            {
                float & alpha = alphaOverrides[iter.first->second];
                alpha = (alpha == emphasisedAlpha ? deemphasisAlpha : emphasisedAlpha);
                atoms[iter.first->second].colour.a = alpha;
                iter.first++;
            }
            elementsNeedUpdate = true;
        }
    }

    return elementsNeedUpdate;
}

/**
 * @brief Set the alpha channel of Atom colours.
 *
 * @param atoms the atoms to set.
 * @param alphas the alphas to set.
 */
void setAlpha(std::vector<Atom> & atoms, std::vector<float> alphas)
{
    for (uint64_t i = 0; i < atoms.size(); i++) { atoms[i].colour.a = alphas[i]; }
}

/**
 * @brief Display a frame of the loading screen.
 *
 * @param display the display to get events from.
 * @param camera the camera for the loading screen.
 * @param loadingAtoms the Atoms to display.
 * @param progressMessage the progress information to draw.
 */
void loadingScreenFrame
(
    jGL::DesktopDisplay & display,
    Camera & camera,
    AtomRenderer & loadingAtoms,
    std::string progressMessage,
    const unsigned int resX,
    const unsigned int resY,
    bool debug = false
)
{
    double deltas[60];
    double delta = 0;
    unsigned frameId = 0;
    auto tic = std::chrono::high_resolution_clock::now();

    jGLInstance->beginFrame();
    jGLInstance->setClear(glm::vec4(1.0f));
    jGLInstance->clear();

    cameraControls(display, camera);

    loadingAtoms.updateCamera(camera);
    loadingAtoms.draw(true);

    std::stringstream debugText;

    debugText << progressMessage;
    if (debug)
    {
        debugText << "\nDelta: " << fixedLengthNumber(delta,6) << " ms"
                  << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                  << ")\n";
    }

    jGLInstance->text(
        debugText.str(),
        glm::vec2(64.0f, resY-64.0f),
        0.5f,
        glm::vec4(0.0f,0.0f,0.0f,1.0f)
    );

    jGLInstance->endFrame();
    display.loop();

    delta = 0.0;
    for (int n = 0; n < 60; n++)
    {
        delta += deltas[n];
    }
    delta /= 60.0;
    auto toc = std::chrono::high_resolution_clock::now();
    deltas[frameId] = std::chrono::duration_cast<std::chrono::milliseconds>(toc-tic).count();
    frameId = (frameId+1) % 60;
}

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

#endif /* MAIN_H */
