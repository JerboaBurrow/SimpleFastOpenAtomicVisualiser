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
#include <util.h>
#include <glUtils.h>
#include <commandLine.h>
#include <xyz.h>
#include <config.h>
#include <camera.h>

const unsigned int resX = 512;
const unsigned int resY = resX;

const float dr = (1.0)*0.5;
const float dtheta = (3.14)*0.025;
const float dphi = (2.0*3.14)*0.05;

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
 * @return true if the Atoms were modified.
 * @return false if the Atoms were not modified.
 */
bool atomControls(jGL::DesktopDisplay & display, std::vector<Atom> & atoms)
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
    return elementsNeedUpdate;
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
    std::string progressMessage
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

    debugText << "Delta: " << fixedLengthNumber(delta,6) << " ms"
                << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                << ")\n"
                << progressMessage;

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

#endif /* MAIN_H */
