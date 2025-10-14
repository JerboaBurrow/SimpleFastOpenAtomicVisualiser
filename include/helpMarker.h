#ifndef HELPMARKER_H
#define HELPMARKER_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#ifdef WINDOWS
#include <imgui/imgui_impl_win32.h>
#endif

#ifdef MACOS
#include <imgui/imgui_impl_osx.h>
#endif

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

#endif /* HELPMARKER_H */
