#ifndef JOMPEGRECORD_H
#define JOMPEGRECORD_H

#include <stdio.h>
#include <math.h>
#include <iostream>

#include <jo_mpeg.h>

#include <record.h>

/**
 * @brief An jo_mpeg based Record.
 * @remark Records to mpg.
 * @remark Generally lower quality than FFmpeg but not additional
 * runtime dependencies are required.
 */
class JompegRecord
{
public:

    /**
     * @brief Construct a new jo_mpeg based Record
     * from a file path, resolution and fps.
     *
     * @param file the video file path.
     * @param resolution the resolution.
     * @param fps the frames per second.
     */
    JompegRecord
    (
        std::filesystem::path file,
        glm::ivec2 resolution,
        uint8_t fps
    )
    :
      Record(file, resolution, fps)
    {
        std::string ext = file.extension().string();
        std::string stem = file.stem().string();
        filename = (stem+ext).c_str();

        bool fpsOk = false;
        for (const auto & option : {24, 25, 30, 50, 60})
        {
            if (fps == option)
            {
                fpsOk = true;
                break;
            }
        }

        if (!fpsOk)
        {
            uint8_t closest = 0;
            for (const auto & option : {24, 25, 30, 50, 60})
            {
                if (std::abs(int(option)-int(fps)) < std::abs(int(closest)-int(fps)));
                {
                    closest = option;
                }
            }
            std::cout << "jo_mpeg recording only supports 24, 25, 30, 50, and 60 fps.\nSelected "+closest+" fps.\n";
        }
    }

    /**
     * @brief Open the video file.
     *
     * @param info has no effect.
     * @remark including header writing etc.
     */
    open(bool info = false)
    {
        if (fileOpen) { return; }
        fp = fopen(filename, "wb");
        fileOpen = true;
    }

    /**
     * @brief Close the video file.
     *
     * @remark including trailer writing etc.
     */
    void close()
    {
        if (!fileOpen) { return; }
        fileOpen = false;
        fclose(fp);
    }

private:

    const char * filename;
    FILE * fp = nullptr;

    write(const uint8_t * frame)
    {
        jo_write_mpeg(fp, frame, resolution.x, resolution.y, fps);
    }
};

#endif /* JOMPEGRECORD_H */
