#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <atom.h>

/**
 * @brief Specification for the structure file interface.
 * @remark @see XYZ for an XYZ/EXTXYZ implementation.
 * @remark @see CONFIG for a CONFIG implementation.
 * @remark Implementors must set:
 *   - atoms: the atom count.
 *   - frames: the frame count.
 *   - linesPerFrame: the (constant) lines in each frame.
 */
class Structure
{
public:

    Structure(std::filesystem::path path)
    : path(path),
      filestream(std::ifstream(path)),
      atoms(0),
      frames(0),
      linesPerFrame(0),
      timeStep(0),
      currentFrame(0),
      linesInFile(0)
    {
        countContentLinesInFile();
    }

    /**
     * @brief Get the number of atoms in the file.
     *
     * @return uint64_t the number of atoms.
     */
    virtual uint64_t atomCount() const { return atoms; }

    /**
     * @brief Read a single frame at position frame, and increment the current frame.
     *
     * @param frame the frame position.
     * @return std::vector<Atom> the Atoms read.
     */
    virtual std::vector<Atom> readFrame(uint64_t frame)
    {
        std::vector<Atom> data;
        frame = frame % frames;
        if (framePositions.find(frame) != framePositions.cend())
        {
            filestream.seekg(framePositions[frame]);
        }
        else
        {
            if (frame == currentFrame)
            {
                framePositions[frame] = filestream.tellg();
            }
            else if (frame > currentFrame)
            {
                skipFrames(frame-currentFrame);
                framePositions[frame] = filestream.tellg();
            }
            else
            {
                beginning();
                if (frame > 0) { skipFrames(frame-1); }
                framePositions[frame] = filestream.tellg();
            }
        }

        getFrame(data);
        currentFrame = frame + 1;
        return data;
    }

    virtual ~Structure() = default;

    /**
     * @brief Get the number of frames.
     *
     * @return uint64_t the frame count.
     */
    uint64_t frameCount() const { return frames; }

    /**
     * @brief Get the current frame index.
     *
     * @return uint64_t the current frame index.
     */
    uint64_t framePosition() const { return currentFrame; }

protected:

    std::filesystem::path path;
    std::ifstream filestream;
    uint64_t atoms;
    uint64_t frames;
    uint64_t linesPerFrame;
    uint64_t timeStep;
    uint64_t currentFrame;
    uint64_t linesInFile;

    std::map<uint64_t, uint64_t> framePositions;

    virtual void beginning()
    {
        filestream.seekg(std::ios::beg);
    }

    virtual void getFrame(std::vector<Atom> & data) = 0;

    void skipFrame()
    {
        if (currentFrame == frames-1) { return; }
        for (uint64_t a = 0; a < linesPerFrame; a++)
        {
            skipLine();
        }
    }

    virtual void skipFrames(uint64_t count)
    {
        if (currentFrame == frames-1) { return; }
        for (uint64_t f = 0; f < count; f++)
        {
            skipFrame();
        }
    }

    void skipLine()
    {
        filestream.ignore
        (
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }

    void cachePositions()
    {
        beginning();
        for (uint64_t f = 0; f < frames; f++)
        {
            framePositions[f] = filestream.tellg();
            skipFrame();
        }
        beginning();
    }

    void countContentLinesInFile()
    {
        filestream.seekg(std::ios::beg);
        linesInFile = 0;
        std::string line;
        // Count lines with content, not by POSIX \n.
        while (std::getline(filestream, line))
        {
            linesInFile++;
        }
        filestream.clear();
        filestream.seekg(std::ios::beg);
    }

    void checkRead
    (
        std::stringstream & ss,
        std::string lastInput,
        std::string context
    )
    {
        if (ss.fail())
        {
            std::stringstream message;
            message << "File "
                    << path.c_str()
                    << " failed to read line"
                    << "\n  Line reads: \""
                    << lastInput
                    << "\""
                    << "\n  Context: "+context;
            throw std::runtime_error
            (
                message.str()
            );
        }
    }
};

#endif /* STRUCTURE_H */
