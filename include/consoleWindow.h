#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#ifdef WINDOWS
#include <imgui/imgui_impl_win32.h>
#endif

#ifdef MACOS
#include <imgui/imgui_impl_osx.h>
#endif

#include <cstdio>
#include <iostream>
#include <sstream>

#include <console.h>
#include <commandLine.h>

/**
 * @brief A window for Console (from Imgui example.)
 *
 */
struct ConsoleWindow
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    bool                  focussed;
    const unsigned logLineSize = 2048;

    std::streambuf * ocout;
    std::streambuf * ocerr;

    std::stringstream out;
    std::stringstream err;

    /**
     * @brief The text banner.
     *
     */
    const char * banner = R"( ________  ________ ________  ________  ___      ___
|\   ____\|\  _____\\   __  \|\   __  \|\  \    /  /|
\ \  \___|\ \  \__/\ \  \|\  \ \  \|\  \ \  \  /  / /
 \ \_____  \ \   __\\ \  \\\  \ \   __  \ \  \/  / /
  \|____|\  \ \  \_| \ \  \\\  \ \  \ \  \ \    / /
    ____\_\  \ \__\   \ \_______\ \__\ \__\ \__/ /
   |\_________\|__|    \|_______|\|__|\|__|\|__|/
   \|_________| SimpleFastOpenAtomicVisualiser

Copyright (C) 2025 Jerboa
SimpleFastOpenAtomicVisualiser comes with ABSOLUTELY NO WARRANTY; for details run GPL.
This is free software, and you are welcome to redistribute it under certain conditions.)";

    /**
     * @brief Construct a new ConsoleWindow
     * @remark redirects cout and cerr to the console window.
     */
    ConsoleWindow()
    {
        ocout = std::cout.rdbuf(out.rdbuf());
        ocerr = std::cerr.rdbuf(err.rdbuf());

        clearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        AutoScroll = true;
        ScrollToBottom = false;
        addLog(std::string(banner));
    }
    ~ConsoleWindow()
    {
        clearLog();
        for (int i = 0; i < History.Size; i++)
        {
            ImGui::MemFree(History[i]);
        }
        std::cout.rdbuf(ocout);
        std::cerr.rdbuf(ocerr);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = std::toupper(*s2) - std::toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = std::toupper(*s2) - std::toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    /**
     * @brief Clear the ConsoleWindow's log.
     *
     */
    void clearLog()
    {
        for (int i = 0; i < Items.Size; i++)
        {
            ImGui::MemFree(Items[i]);
        }
        Items.clear();
    }

    /**
     * @brief Add a const char * to the log.
     */
    void addLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        char buf[logLineSize];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    /**
     * @brief Add a string to the log.
     * @param s the string to add.
     */
    void addLog(std::string s)
    {
        if (s.size() <= logLineSize) { addLog("%s", s.c_str()); }
        else
        {
            for (unsigned i = 0; i < s.size(); i += logLineSize)
            {
                std::cout << s.substr(i, logLineSize).size() << "\n";
                addLog("%s", s.substr(i, logLineSize).c_str());
            }
        }
    }

    /**
     * @brief Draw the console window.
     *
     * @param title the name of the window.
     * @param console the Console to execute commands with.
     */
    void draw(const char* title, Console & console)
    {
        std::string line;

        while (std::getline(out,line)) { addLog(line); }
        out.clear();

        while (std::getline(err,line)) { addLog("[ERROR] "+line); }
        err.clear();

        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title))
        {
            ImGui::End();
            return;
        }

        focussed = ImGui::IsWindowFocused();

        ImGui::TextWrapped("Enter 'help()' for a list of functions.");
        ImGui::TextWrapped("For help with each function: 'help(zoomCamera)'.");

        if (ImGui::SmallButton("Clear")) { clearLog(); }

        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) { clearLog(); }
                ImGui::EndPopup();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

            for (const char* item : Items)
            {
                if (!Filter.PassFilter(item))
                {
                    continue;
                }
                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color) { ImGui::PushStyleColor(ImGuiCol_Text, color); }
                ImGui::TextWrapped("%s", item);
                if (has_color) { ImGui::PopStyleColor(); }
            }

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) { ImGui::SetScrollHereY(1.0f); }
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0]) { execCommand(s, console); }
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        // Auto focus previous widget
        if (reclaim_focus) { ImGui::SetKeyboardFocusHere(-1); }

        ImGui::End();
    }

    /**
     * @brief Execute a console command.
     *
     * @param command_line the command.
     * @param console the Console to execute with.
     */
    void execCommand(const char* command_line, Console & console)
    {
        addLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
        {
            if (Stricmp(History[i], command_line) == 0)
            {
                ImGui::MemFree(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            clearLog();
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
            {
                addLog("%3d: %s\n", i, History[i]);
            }
        }
        else
        {
            console.runString(command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ConsoleWindow* console = (ConsoleWindow*)data->UserData;
        return console->textEditCallback(data);
    }

    int textEditCallback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackCompletion:
                {
                    // Example of TEXT COMPLETION

                    // Locate beginning of current word
                    const char* word_end = data->Buf + data->CursorPos;
                    const char* word_start = word_end;
                    while (word_start > data->Buf)
                    {
                        const char c = word_start[-1];
                        if (c == ' ' || c == '\t' || c == ',' || c == ';') { break; }
                        word_start--;
                    }

                    // Build a list of candidates
                    ImVector<const char*> candidates;
                    for (int i = 0; i < Commands.Size; i++)
                    {
                        if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        {
                            candidates.push_back(Commands[i]);
                        }
                    }
                    if (candidates.Size == 0)
                    {
                        // No match
                        addLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                    }
                    else if (candidates.Size == 1)
                    {
                        // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0]);
                        data->InsertChars(data->CursorPos, " ");
                    }
                    else
                    {
                        // Multiple matches. Complete as much as we can..
                        // So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                        int match_len = (int)(word_end - word_start);
                        for (;;)
                        {
                            int c = 0;
                            bool all_candidates_matches = true;
                            for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            {
                                if (i == 0)
                                {
                                    c = std::toupper(candidates[i][match_len]);
                                }
                                else if (c == 0 || c != std::toupper(candidates[i][match_len]))
                                {
                                    all_candidates_matches = false;
                                }
                            }
                            if (!all_candidates_matches) { break; }
                            match_len++;
                        }

                        if (match_len > 0)
                        {
                            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                            data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                        }

                        // List matches
                        addLog("Possible matches:\n");
                        for (int i = 0; i < candidates.Size; i++)
                        {
                            addLog("- %s\n", candidates[i]);
                        }
                    }

                    break;
                }
            case ImGuiInputTextFlags_CallbackHistory:
                {
                    // Example of HISTORY
                    const int prev_history_pos = HistoryPos;
                    if (data->EventKey == ImGuiKey_UpArrow)
                    {
                        if (HistoryPos == -1) { HistoryPos = History.Size - 1; }
                        else if (HistoryPos > 0) { HistoryPos--; }
                    }
                    else if (data->EventKey == ImGuiKey_DownArrow)
                    {
                        if (HistoryPos != -1)
                        {
                            if (++HistoryPos >= History.Size)
                            {
                                HistoryPos = -1;
                            }
                        }
                    }

                    // A better implementation would preserve the data on the current input line along with cursor position.
                    if (prev_history_pos != HistoryPos)
                    {
                        const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, history_str);
                    }
                }
        }
        return 0;
    }
};

#endif /* CONSOLEWINDOW_H */
