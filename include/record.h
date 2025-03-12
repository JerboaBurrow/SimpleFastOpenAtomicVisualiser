#ifndef RECORD_H
#define RECORD_H

#include <filesystem>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

/**
 * @brief An API for recording video.
 *
 */
class Record
{
public:

    /**
     * @brief Construct a new Record from a file path, resolution and fps.
     *
     * @param file the video file path.
     * @param resolution the resolution.
     * @param fps the frames per second.
     */
    Record(std::filesystem::path file, glm::ivec2 resolution, uint8_t fps)
    :   file(file),
        resolution(resolution),
        fps(fps)
        {}

    ~Record(){}

    /**
     * @brief Open the video file.
     *
     * @param info print extra information.
     * @remark including header writing etc.
     */
    virtual void open(bool info = false) = 0;

    /**
     * @brief Enqueue a frame to be written.
     *
     * @param pixels the frame's pixels.
     */
    void queueFrame(std::vector<uint8_t> pixels)
    {
        if (fileOpen) { inBuffer.push_back(pixels); }
    }

    uint8_t queueSize() { return inBuffer.size(); }

    void writeFrames()
    {
        if (writing || inBuffer.size() == 0) { return; }
        outBuffer = inBuffer;
        inBuffer.clear();
        std::thread io = std::thread
        (
            &Record::writeBuffer,
            this
        );
        io.detach();
    }

    /**
     * @brief Close the video file.
     *
     * @remark including trailer writing etc.
     * @warning See isWriting to check if the threaded
     * io is still active.
     */
    virtual void close() = 0;

    bool isWriting() const { return writing; }

    bool isOpen() const { return fileOpen; }

protected:

    std::filesystem::path file;
    glm::ivec2 resolution;
    uint8_t fps;
    bool fileOpen = false;
    bool writing = false;

    std::vector<std::vector<uint8_t>> inBuffer, outBuffer;

    virtual void write(const uint8_t * frame) = 0;

    void writeBuffer()
    {
        writing = true;
        for (const auto & frame : outBuffer)
        {
            write(frame.data());
        }
        outBuffer.clear();
        writing = false;
    }
};

#endif /* RECORD_H */
