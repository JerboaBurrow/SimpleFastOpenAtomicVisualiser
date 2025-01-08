#ifndef DISPLAY_H
#define DISPLAY_H

#include <glm/glm.hpp>
#include <chrono>
#include <thread>
#include <iostream>

namespace jGL
{
    class Display
    {
    
    public:

        Display(glm::ivec2 res)
        : 
            resolution(res), 
            unlimited(true), 
            framesPerSecond(-1),
            microsPerFrame(0),
            lastFrame(std::chrono::steady_clock::now())
        {}

        unsigned getResX() const { return resolution.x; }
        unsigned getResY() const { return resolution.y; }
        glm::ivec2 getRes() const { return resolution; }

        virtual void setFrameLimit(unsigned fps)
        {
            framesPerSecond = fps;
            microsPerFrame = std::chrono::microseconds(int(1000000.0 / double(fps)));
            unlimited = false;
        }

        virtual void loop()
        {
            throttle();
        }
        
    protected:

        glm::ivec2 resolution;
        bool unlimited;
        unsigned framesPerSecond;
        std::chrono::microseconds microsPerFrame;
        std::chrono::steady_clock::time_point lastFrame;

        virtual void throttle()
        {   
            if (unlimited)
            {
                return;
            }
            std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
            std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t-lastFrame);
            if (elapsed < microsPerFrame)
            {
                std::this_thread::sleep_for(microsPerFrame-elapsed);
            }
            lastFrame = std::chrono::steady_clock::now();
        }

    };
}

#endif /* DISPLAY_H */
