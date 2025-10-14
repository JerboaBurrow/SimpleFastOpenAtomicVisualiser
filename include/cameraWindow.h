#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

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
#include <commandLine.h>
#include <atom.h>
#include <helpMarker.h>

class CameraWindow
{
public:

    struct CameraUpdates
    {
        CameraUpdates()
        : cameraMoved(false),
          atomsMoved(false)
        {}

        bool cameraMoved;
        bool atomsMoved;
    };

    CameraWindow() {}

    void draw
    (
        CommandLine & options
    )
    {
        ImGui::Begin("Camera");
        ImGui::Text("Zoom    ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##zoomin", ImGuiDir_Up)) { zoomQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##zoomout", ImGuiDir_Down)) { zoomQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Zoom to and from the focus (W/S)");

        ImGui::Text("Rotate  ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##rotatel", ImGuiDir_Left)) { rotateQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##rotater", ImGuiDir_Right)) { rotateQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Rotate around the focus (A/D)");

        ImGui::Text("Incline ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##inclineu", ImGuiDir_Up)) { inclineQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##inclined", ImGuiDir_Down)) { inclineQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Incline around the focus (Q/E)");

        ImGui::Text("Pan x   ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##panx-", ImGuiDir_Left)) { panxQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##panx+", ImGuiDir_Right)) { panxQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Pan in x (Left/Right)");

        ImGui::Text("Pan y   ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##pany-", ImGuiDir_Left)) { panyQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##pany+", ImGuiDir_Right)) { panyQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Pan in y (Up/Down)");

        ImGui::Text("Pan z   ");
        ImGui::SameLine();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##panz-", ImGuiDir_Left)) { panzQueue--; }
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        if (ImGui::ArrowButton("##panz+", ImGuiDir_Right)) { panzQueue++; }
        ImGui::PopItemFlag();
        ImGui::SameLine(); HelpMarker("Pan in z (.//)");

        ImGui::PushItemWidth(200);
        ImGui::InputFloat("Zoom speed:    ", &options.cameraZoomSpeed.value, 0.01f, 1.0f, "%.2f");
        ImGui::SameLine(); HelpMarker("Modify zoom rate (CTRL-CLICK for faster steps)");

        ImGui::PushItemWidth(200);
        ImGui::InputFloat("Rotate speed:  ", &options.cameraRotateSpeed.value, 0.01f, 1.0f, "%.2f");
        ImGui::SameLine(); HelpMarker("Modify rotate rate (CTRL-CLICK for faster steps)");

        ImGui::PushItemWidth(200);
        ImGui::InputFloat("Incline speed: ", &options.cameraInclineSpeed.value, 0.01f, 1.0f, "%.2f");
        ImGui::SameLine(); HelpMarker("Modify inclination rate (CTRL-CLICK for faster steps)");

        ImGui::PushItemWidth(200);
        ImGui::InputFloat("Pan speed:     ", &options.cameraPanSpeed.value, 0.01f, 1.0f, "%.2f");
        ImGui::SameLine(); HelpMarker("Modify pan rate (CTRL-CLICK for faster steps)");

        if (ImGui::Button("Reset Camera   "))
        {
            resetCamera = true;
        }

        ImGui::End();
    }

    CameraUpdates applyQueueActions
    (
        Camera & camera,
        std::vector<Atom> & atoms,
        const CommandLine & options,
        const float & dr,
        const float & dtheta,
        const float & dphi
    )
    {
        CameraUpdates moved;

        if (zoomQueue != 0)
        {
            camera.zoom(dr*options.cameraZoomSpeed.value*zoomQueue);
            zoomQueue = 0;
            moved.cameraMoved = true;
        }

        if (rotateQueue != 0)
        {
            camera.rotate(dtheta*options.cameraRotateSpeed.value*rotateQueue);
            rotateQueue = 0;
            moved.cameraMoved = true;
        }

        if (inclineQueue != 0)
        {
            camera.incline(dphi*options.cameraInclineSpeed.value*inclineQueue);
            inclineQueue = 0;
            moved.cameraMoved = true;
        }

        if (resetCamera)
        {
            if (!options.noCentering.value) { center(atoms); }
            if (options.focus.value < atoms.size()) { centerOn(atoms, options.focus.value); }
            camera.reset(atoms);
            moved.atomsMoved = true;
            moved.cameraMoved = true;
            resetCamera = false;
        }

        if (panxQueue != 0)
        {
            translate(atoms, {dr*options.cameraPanSpeed.value*panxQueue, 0.0, 0.0});
            panxQueue = 0;
            moved.atomsMoved = true;
        }

        if (panyQueue != 0)
        {
            translate(atoms, {0.0, dr*options.cameraPanSpeed.value*panyQueue, 0.0});
            panyQueue = 0;
            moved.atomsMoved = true;
        }

        if (panzQueue != 0)
        {
            translate(atoms, {0.0, 0.0, dr*options.cameraPanSpeed.value*panzQueue});
            panzQueue = 0;
            moved.atomsMoved = true;
        }

        return moved;
    }

private:

    inline static int zoomQueue = 0;
    inline static int rotateQueue = 0;
    inline static int inclineQueue = 0;
    inline static int panxQueue = 0;
    inline static int panyQueue = 0;
    inline static int panzQueue = 0;
    inline static bool resetCamera = false;

};

#endif /* CAMERAWINDOW_H */
