#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <algorithm>
#include <exception>

#include <vendored/jThread/jThread.h>

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

    Structure(std::filesystem::path path, bool blocking = false)
    : path(path),
      blockingReads(blocking),
      filestream(std::ifstream(path)),
      natoms(0),
      frames(0),
      linesPerFrame(0),
      timeStep(0),
      currentFrame(0),
      linesInFile(0),
      atomsRead(0),
      cellA(0),
      cellB(0),
      cellC(0)
    {}

    /**
     * @brief Get the number of atoms in the file.
     *
     * @return uint64_t the number of atoms.
     */
    virtual uint64_t atomCount() const { return atoms.size(); }

    /**
     * @brief Read a single frame at position frame, and increment the current frame.
     *
     * @remark Structure::frameCount is the maximum readable frame.
     * If frame is larger it will be %'d.
     *
     * @see framePosition for the status of detached frame caching.
     * @see getAtoms for accessing the read data.
     *
     * @param frame the frame position.
     */
    virtual void readFrame(uint64_t frame)
    {
        if (atoms.size() != natoms) { atoms.resize(natoms); }
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

        getFrame();
        currentFrame = frame + 1;
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

    /**
     * @brief Check if frames start positions have been loaded.
     *
     * @remark Frame start position offsets are loaded in a
     * detached I/O thread. The result of this method indicates
     * whether the I/O read has completed.
     *
     * @remark Structure::frameCount will be update while the
     * detached read is progressing.
     *
     * @remark Structure::readFrame will only allow reads up to
     * Structure::framePosition.
     *
     * @return true if all frame start offsets have been loaded.
     * @return false frame start positions are still being read.
     */
    bool framePositionsLoaded() const { return cacheComplete; }

    /**
     * @brief Progress of the current frame read.
     *
     * @return uint64_t count of Atom read into atoms.
     */
    uint64_t frameReadProgress() const { return atomsRead; }

    /**
     * @brief If the frame has been fully read into atoms.
     *
     * @return true if all Atoms in the frame have been read.
     * @return false if reading is in progress.
     */
    bool frameReadComplete() const { return atomsRead == atoms.size(); }

    /**
     * @brief The Atoms read in the current frame.
     *
     */
    std::vector<Atom> atoms;

protected:

    std::filesystem::path path;
    bool blockingReads;
    std::ifstream filestream;
    uint64_t natoms;
    uint64_t frames;
    uint64_t linesPerFrame;
    uint64_t timeStep;
    uint64_t currentFrame;
    uint64_t linesInFile;
    uint64_t atomsRead;

    glm::vec3 cellA;
    glm::vec3 cellB;
    glm::vec3 cellC;

    bool cacheComplete = false;

    std::map<uint64_t, uint64_t> framePositions;

    virtual void beginning()
    {
        filestream.seekg(std::ios::beg);
    }

    virtual void getFrame() = 0;

    virtual void getCell() = 0;

    void skipFrame()
    {
        if (currentFrame == frames-1) { return; }
        for (uint64_t a = 0; a < linesPerFrame; a++)
        {
            skipLine(filestream);
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

    void skipLine(std::ifstream & in)
    {
        in.ignore
        (
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }

    virtual void initialise() = 0;

    void scanPositions()
    {
        if (blockingReads) { cachePositions(); return; }
        // Non-blocking read of latter frame positions.
        std::thread io = std::thread
        (
            &Structure::cachePositions,
            this
        );
        io.detach();
    }

    void cachePositions()
    {
        cacheComplete = false;
        std::ifstream scanFileStream(path);
        scanFileStream.seekg(framePositions[0]);
        uint64_t f = 1;
        for (uint64_t a = 0; a < linesPerFrame; a++)
        {
            skipLine(scanFileStream);
        }
        while (scanFileStream.peek() != EOF)
        {
            framePositions[f] = scanFileStream.tellg();
            f++;
            frames = f;
            for (uint64_t a = 0; a < linesPerFrame; a++)
            {
                skipLine(scanFileStream);
            }
        }
        cacheComplete = true;
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
