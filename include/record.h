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

    virtual ~Record(){}

    /**
     * @brief Open the video file.
     *
     * @remark including header writing etc.
     */
    virtual void open() = 0;

    /**
     * @brief Enqueue a frame to be written.
     *
     * @param pixels the frame's pixels.
     */
    void queueFrame(std::vector<uint8_t> pixels)
    {
        if (fileOpen) { inBuffer.push_back(pixels); }
    }

    /**
     * @brief Number of frames yet to be sent for writing.
     *
     * @return uint8_t the count.
     */
    uint8_t queueSize() { return inBuffer.size(); }

    /**
     * @brief Number of frame left to be written in total.
     *
     * @return uint64_t total count.
     */
    uint64_t framesLeft() { return inBuffer.size() + outBuffer.size(); }

    /**
     * @brief Send the frames in the buffer for writing.
     * @remark Runs in a background thread, sets a flag to
     * stop additional threads spawning.
     */
    void writeFrames()
    {
        if (writing || inBuffer.size() == 0) { return; }
        writing = true;
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

    /**
     * @brief If the video is being written to in the background.
     *
     * @return true writing is happening.
     * @return false no writing is happening.
     */
    bool isWriting() const { return writing; }

    /**
     * @brief If the file is open.
     *
     * @return true the video file is open.
     * @return false the video file is not open.
     */
    bool isOpen() const { return fileOpen; }

    /**
     * @brief Check if finalised.
     *
     * @return true all writing is finished, the file is closed.
     * @return false either writing is happening or queued.
     */
    bool finalise()
    {
        if (!isWriting())
        {
            if (queueSize() > 0)
            {
                writeFrames();
            }
            else
            {
                close();
                return true;
            }
        }
        return false;
    }

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
        for (const auto & frame : outBuffer)
        {
            write(frame.data());
        }
        outBuffer.clear();
        writing = false;
    }
};

#endif /* RECORD_H */
