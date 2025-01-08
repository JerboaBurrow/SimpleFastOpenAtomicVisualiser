#ifndef DesktopDisplay_H
#define DesktopDisplay_H

#ifndef ANDROID

#include <cstdlib>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <jGL/Display/event.h>
#include <jGL/Display/display.h>

#include <jGL/common.h>
#include <stb_image.h>
#include <algorithm>

#ifdef WINDOWS
#include <windows.h>
#include <mmsystem.h>
#endif

namespace jGL
{

    void defaultKeyEventCallback
    (
        GLFWwindow * window,
        int key,
        int scancode,
        int action,
        int mods
    );

    class DesktopDisplay : public Display
    {
    public:

        struct Config
        {
            Config()
            : VULKAN(false), 
              COCOA_RETINA(false), 
              CLIP_TO_MONITOR(true),
              CLIP_TO_WORK_AREA(true)
            {}
            
            Config
            (
                bool vulkan, 
                bool cocoa, 
                bool clipMonitor, 
                bool clipWorkArea
            )
            : VULKAN(vulkan), 
              COCOA_RETINA(cocoa),
              CLIP_TO_MONITOR(clipMonitor),
              CLIP_TO_WORK_AREA(clipWorkArea)
            {}

            Config (const Config & c)
            : VULKAN(c.VULKAN), 
              COCOA_RETINA(c.COCOA_RETINA),
              CLIP_TO_MONITOR(c.CLIP_TO_MONITOR),
              CLIP_TO_WORK_AREA(c.CLIP_TO_WORK_AREA)
            {}

            bool VULKAN;
            bool COCOA_RETINA;
            // if screen size bigger than monitor clip
            bool CLIP_TO_MONITOR;
            // if screen size too big to fit with taskbars, cip
            bool CLIP_TO_WORK_AREA;
        };

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            GLFWkeyfun keyCallback,
            GLFWmousebuttonfun mouseButtonCallback,
            GLFWscrollfun mouseScrollCallback,
            const Config conf
        );

        DesktopDisplay
        (
            glm::ivec2 res,
            const char * title,
            const Config conf
        );

        ~DesktopDisplay()
        {
            for (auto icon : logo)
            {
                stbi_image_free(icon.pixels);
            }
            logo.clear();

            glfwTerminate();
            #ifdef WINDOWS
                timeEndPeriod(1);
            #endif
        }

        GLFWwindow * getWindow() const { return glfwWindow; }

        bool isOpen(){ if (glfwWindow != NULL) { return !glfwWindow ? false : true; } return false; }
        bool closing(){ return glfwWindowShouldClose(glfwWindow); }
        
        void open(){ 
            #ifdef WINDOWS
                timeBeginPeriod(1);
            #endif
            if (glfwWindow == NULL)
            { 
                // required for MacOS
                //   https://www.glfw.org/faq.html#41__how_do_i_create_an_opengl_30_context
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, windowConfig.COCOA_RETINA);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

                // get work area (i.e. without taskbars)
                int wxpos, wypos, wwidth, wheight;
                glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &wxpos, &wypos, &wwidth, &wheight);

                // hack to obtain decoration size
                GLFWwindow * temporaryWindow = glfwCreateWindow(1, 1, "", NULL, NULL);
                int fleft, ftop, fright, fbottom;
                glfwGetWindowFrameSize(temporaryWindow, &fleft, &ftop, &fright, &fbottom);
                glfwWindowShouldClose(temporaryWindow);
                glfwDestroyWindow(temporaryWindow);

                #ifdef WINDOWS
                    // windows pos includes decoration...
                    glm::ivec2 pos(wxpos, wypos+ftop);
                #else
                    glm::ivec2 pos(wxpos, wypos);
                #endif

                if (windowConfig.CLIP_TO_MONITOR)
                {
                    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                    resolution.x = std::min(resolution.x, mode->width);
                    resolution.y = std::min(resolution.y, mode->height);
                }

                if (windowConfig.CLIP_TO_WORK_AREA)
                {
                    if (resolution.y+ftop > wheight)
                    {
                        resolution.y = wheight-ftop;
                    }

                    if (resolution.x > wwidth)
                    {
                        resolution.x = wwidth;
                    }
                }
                
                glfwWindow = glfwCreateWindow(getResX(), getResY(),title,NULL,NULL); 
                glfwSwapInterval(1); 
                setWindowPosition(pos);
            } 
        }

        void close(){ if (glfwWindow != NULL) { glfwDestroyWindow(glfwWindow); glfwWindow = NULL; } }

        void setAsFocus(){ if (glfwWindow != NULL) { glfwMakeContextCurrent(glfwWindow); } }

        void keyCallback
        (
            GLFWwindow * window,
            int key,
            int scancode,
            int action,
            int mods
        );

        void mousePosition(double & x, double & y){ if (glfwWindow != NULL){ glfwGetCursorPos(glfwWindow,&x,&y); } }

        void setMousePosition(double x, double y)
        {
            if (glfwWindow != NULL)
            {
                glfwSetCursorPos(glfwWindow, x, y);
            }
        }

        int getKeyLastState(int key) { return glfwGetKey(glfwWindow, key); }

        void loop() 
        { 
            data.clear();
            handleEvents();
            if (glfwWindowShouldClose(glfwWindow)){ close(); }
            throttle();
            swap(); 
        }

        std::vector<Event> getEvents(int code) 
        {
            if (data.events.find(code) == data.events.cend())
            {
                return {Event()};
            }
            else
            {
                return data.events[code];
            }
        }

        bool keyHasEvent(int key, EventType action) 
        {
            if (data.events.find(key) == data.events.cend())
            {
                return false;
            }
            else
            {
                auto ts = getEventTypes(key);
                return std::find(ts.cbegin(), ts.cend(), action) != ts.cend(); 
            }
        }

        std::vector<EventType> getEventTypes(int code) 
        {
            std::vector<EventType> e;
            if (data.events.find(code) == data.events.cend())
            {
                return {EventType::NONE};
            }
            else
            {
                for (auto evt : data.events[code])
                {
                    e.push_back(evt.type);
                }
                return e;
            }
        }

        Event getEvent(int code)
        {
            if (data.events.find(code) == data.events.cend())
            {
                return Event();
            }
            else
            {
                return data.events[code][0];
            }
        }

        struct WindowData 
        {
            std::map<int, std::vector<Event>> events;
            double scrollX = 0.0;
            double scrollY = 0.0;
            bool scrolled = false;

            void clear()
            {
                scrollX = 0.0;
                scrollY = 0.0;
                scrolled = false;
                events.clear();
            }
        };

        void setIcon(const std::vector<std::vector<std::byte>> & icons)
        {
            glfwSetWindowIcon(glfwWindow, 0, NULL);
            logo.clear();
            for (auto icon : icons)
            {
                GLFWimage image;
                unsigned char * chData = reinterpret_cast<unsigned char*>(icon.data());
                image.pixels = stbi_load_from_memory
                (
                    chData, 
                    icon.size(), 
                    &image.width, 
                    &image.height, 
                    0, 
                    4
                );
                logo.push_back(image);
            }

            glfwSetWindowIcon(glfwWindow,logo.size(),logo.data());
        }

        glm::ivec2 frameBufferSize() const 
        {
            glm::ivec2 s;
            if (glfwWindow != NULL)
            {
                glfwGetFramebufferSize(glfwWindow, &s.x, &s.y);
            }
            return s;
        }

        glm::ivec2 windowSize() const 
        {
            glm::ivec2 s;
            if (glfwWindow != NULL)
            {
                glfwGetWindowSize(glfwWindow, &s.x, &s.y);
            }
            return s;
        }

        glm::vec2 contentScale() const
        {
            glm::vec2 s;
            if (glfwWindow != NULL)
            {
                glfwGetWindowContentScale(glfwWindow, &s.x, &s.y);
            }
            return s;
        }

        glm::ivec4 windowFrameSize() const 
        {
            glm::ivec4 s;
            if (glfwWindow != NULL)
            {
                glfwGetWindowFrameSize(glfwWindow, &s.x, &s.y, &s.z, &s.w);
            }
            return s;
        }

        glm::ivec2 windowPosition() const 
        {
            glm::ivec2 s;
            if (glfwWindow != NULL)
            {
                glfwGetWindowPos(glfwWindow, &s.x, &s.y);
            }
            return s;
        }

        void setWindowPosition(glm::ivec2 s) 
        {
            if (glfwWindow != NULL)
            {
                glfwSetWindowPos(glfwWindow, s.x, s.y);
            }
        }

    private:

        const char * title;

        std::vector<GLFWimage> logo;

        GLFWwindow * glfwWindow;

        WindowData data;

        Config windowConfig;

        void swap(){ if (glfwWindow != NULL) { glfwSwapBuffers(glfwWindow); } }

        void handleEvents(){ if (glfwWindow != NULL){ glfwPollEvents(); } }

    };
}

#endif /* ANDROID */
#endif /* DesktopDisplay_H */