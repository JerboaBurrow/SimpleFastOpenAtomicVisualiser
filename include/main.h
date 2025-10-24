#ifndef MAIN_H
#define MAIN_H

#include <sstream>
#include <chrono>
#include <exception>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <icon.h>

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
#include <console.h>
#include <visualisationState.h>
#include <neighbours.h>
#include <infoWindow.h>
#include <cameraWindow.h>
#include <font.h>

const float dr = (1.0)*0.5;
const float dtheta = (3.14)*0.025;
const float dphi = (2.0*3.14)*0.05;

const float emphasisedAlpha = 1.0f;

bool closing = false;

std::unique_ptr<jGL::jGLInstance> jGLInstance;

/**
 * @brief Obtain screen pixels and write to a png.
 *
 * @param resolution the screen resolution.
 * @remark The file name is a timestamp.
 */
void screenshot(glm::ivec2 resolution)
{
    std::vector<uint8_t> pixels(resolution.x*resolution.y*4, 0);
    glReadPixels
    (
        0,
        0,
        resolution.x,
        resolution.y,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );
    stbi_flip_vertically_on_write(true);
    std::string name = timeStamp()+".png";
    stbi_write_png(name.c_str(), resolution.x, resolution.y, 4, pixels.data(), 4*resolution.x);
}

/**
 * @brief Override jGL default event callback.
 * @remark So window closing can account for video writing.
 *
 * @param window the active GLFWwindow.
 * @param key the key code.
 * @param scancode the key's scancode.
 * @param action the action performed.
 * @param mods mods.
 */
void keyEventCallback
(
    GLFWwindow * window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        closing = true;
    }
    jGL::parseAction(window, key, action);
}

/**
 * @brief A background and text colour theme.
 *
 */
struct Theme
{
    glm::vec4 background;
    glm::vec4 text;
};

/**
 * @brief A dark colour theme.
 *
 * @return the Theme.
 */
Theme darkTheme()
{
    return
    {
        glm::vec4(5.0, 5.0, 5.0, 255.0)/255.0f,
        glm::vec4(250.0, 250.0, 250.0, 255.0)/255.0f
    };
}

/**
 * @brief A light colour theme.
 *
 * @return the Theme.
 */
Theme lightTheme()
{
    return
    {
        glm::vec4(250.0, 250.0, 250.0, 255.0)/255.0f,
        glm::vec4(5.0, 5.0, 5.0, 255.0)/255.0f
    };
}

/**
 * @brief Controls for the camera
 *
 * @param display display to obtain events from.
 * @param camera the camera to update.
 * @param zoomSpeed multiplier for zoom.
 * @param rotateSpeed multiplier for rotation.
 * @param inclineSpeed multiplier for inclination.
 * @return true if the camera moved.
 * @return false if the camera did not move.
 */
void cameraControls
(
    jGL::DesktopDisplay & display,
    Camera & camera,
    float zoomSpeed,
    float rotateSpeed,
    float inclineSpeed
)
{
    if (display.keyHasAnyEvents(GLFW_KEY_W, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.zoom(-dr*zoomSpeed);
    }
    if (display.keyHasAnyEvents(GLFW_KEY_S, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.zoom(dr*zoomSpeed);
    }
    if (display.keyHasAnyEvents(GLFW_KEY_Q, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.incline(dtheta*inclineSpeed);
    }
    if (display.keyHasAnyEvents(GLFW_KEY_E, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.incline(-dtheta*inclineSpeed);
    }
    if (display.keyHasAnyEvents(GLFW_KEY_A, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.rotate(-dphi*rotateSpeed);
    }
    if (display.keyHasAnyEvents(GLFW_KEY_D, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
    {
        camera.rotate(dphi*rotateSpeed);
    }
}

/**
 * @brief Controls for the Atoms position's
 *
 * @param display the display to get events from.
 * @param atoms the Atoms to modify.
 * @param emphasisControls key bindings to emphasise elements.
 * @param elementMap map elements to indices.
 * @param alphaOverrides overriding alpha channels.
 * @param deemphasisAlpha alpha for faded atoms.
 * @param translateSpeed atom translation speed.
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
    float deemphasisAlpha,
    float translateSpeed
)
{
    bool elementsNeedUpdate = false;
    if (display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::HOLD))
    {
        translate(atoms, {-dr*translateSpeed, 0.0, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::HOLD))
    {
        translate(atoms, {dr*translateSpeed, 0.0, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, -dr*translateSpeed, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, dr*translateSpeed, 0.0});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, 0.0, -dr*translateSpeed});
        elementsNeedUpdate = true;
    }
    if (display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::HOLD))
    {
        translate(atoms, {0.0, 0.0, dr*translateSpeed});
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
    Theme theme,
    const unsigned int resX,
    const unsigned int resY,
    bool hideInfo = false
)
{
    double deltas[60];
    double delta = 0;
    unsigned frameId = 0;
    auto tic = std::chrono::high_resolution_clock::now();

    jGLInstance->beginFrame();
    jGLInstance->setClear(theme.background);
    jGLInstance->clear();

    cameraControls(display, camera, 1.0f, 1.0f, 1.0f);

    loadingAtoms.updateCamera(camera);
    loadingAtoms.draw(true);

    std::stringstream debugText;

    if (!hideInfo)
    {
        jGLInstance->text(
            progressMessage,
            glm::vec2(64.0f, resY-64.0f),
            0.5f,
            theme.text
        );
    }

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
 * @brief Move back one frame.
 *
 * @param structure the Structure file.
 */
void backward(std::unique_ptr<Structure> & structure)
{
    uint64_t f = structure->framePosition();
    if (f > 2) { f -= 2; }
    else { f = structure->frameCount()-2+f;}
    structure->readFrame(f);
}

/**
 * @brief Enable of disable transparency sorting if there are transparent elements.
 *
 * @param atoms the Atoms to check.
 * @param atomRenderer the AtomRenderer to enable/disable transparency sorting on.
 * @param bondRenderer the BondRenderer to enable/disable transparency sorting on.
 */
void setTransparencySorting
(
    const std::vector<Atom> & atoms,
    AtomRenderer & atomRenderer,
    BondRenderer & bondRenderer
)
{
    for (const auto & atom : atoms)
    {
        if (atom.colour.a != 0.0 && atom.colour.a != 1.0)
        {
            atomRenderer.setTransparencySorting(true);
            bondRenderer.setTransparencySorting(true);
            return;
        }
    }
    atomRenderer.setTransparencySorting(false);
    bondRenderer.setTransparencySorting(false);
}

/**
 * @brief Loads fonts for ImGui.
 *
 */
void loadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_cfg;
    io.Fonts->AddFontFromMemoryTTF(OpenDyslexicMono, sizeof(OpenDyslexicMono), 20.0);
}

#endif /* MAIN_H */
