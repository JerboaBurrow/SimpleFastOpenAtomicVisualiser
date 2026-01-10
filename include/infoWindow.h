#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <memory.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#ifdef WINDOWS
#include <imgui/imgui_impl_win32.h>
#endif

#ifdef MACOS
#include <imgui/imgui_impl_osx.h>
#endif

#include <camera.h>
#include <visualisationState.h>
#include <structure.h>
#include <atomRenderer.h>
#include <bondRenderer.h>
#include <commandLine.h>

/**
 * @brief Draw an information window.
 *
 * @param structure the Structure to get info from.
 * @param camera the Camera to get info from.
 * @param visualisationState the VisualisationState to get info from.
 * @param delta the frame delta in milliseconds.
 */
void infoWindow
(
    const std::unique_ptr<Structure> & structure,
    const Camera & camera,
    const VisualisationState & visualisationState,
    const float delta
)
{
    auto cx = fixedLengthNumber(camera.position().x, 6);
    auto cy = fixedLengthNumber(camera.position().y, 6);
    auto cz = fixedLengthNumber(camera.position().z, 6);

    auto ift = fixedLengthNumber(delta,6);
    auto fps = fixedLengthNumber(1.0/(delta*1e-3),4);

    ImGui::Begin("Information");

    unsigned size = std::ceil(structure->frameCount()/10.0);
    ImGui::Text("Structure: %s", structure->getPath().c_str());
    ImGui::Text("Frame %*lu / %*lu", size, visualisationState.frame+1, size, structure->frameCount());
    ImGui::Text("Frame caching %s", (structure->framePositionsLoaded() ? "complete." : "in progress."));
    ImGui::Text("Camera %s, %s, %s", cx.c_str(), cy.c_str(), cz.c_str());
    ImGui::Text("Frame Time (FPS) %s ms (%s)", ift.c_str(), fps.c_str());

    ImGui::End();
}

#endif /* INFOWINDOW_H */
