#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <atom.h>

/**
 * @brief Specification for the structure file interface.
 * @remark @see XYZ for an XYZ/EXTXYZ implementation.
 * @remark @see CONFIG for a CONFIG implementation.
 */
class Structure
{
public:

    Structure(std::filesystem::path path)
    : path(path),
      filestream(std::ifstream(path)),
      currentFrame(0)
    {
      countLinesInFile();
    }

    /**
     * @brief Get the number of atoms in the file.
     *
     * @return uint64_t the number of atoms.
     */
    virtual uint64_t atomCount() const { return atoms; }


    /**
     * @brief Read a single frame at position frame.
     *
     * @param frame the frame position.
     * @return std::vector<Atom> the Atoms read.
     */
    /**
     * @brief Read a single frame at position frame.
     *
     * @param frame the frame position.
     * @return std::vector<Atom> the Atoms read.
     */
    virtual std::vector<Atom> readFrame(uint64_t frame)
    {
        std::vector<Atom> data;
        if (frame > frames-1) { return data; }
        if (frame == currentFrame)
        {
            getFrame(data);
        }
        else if (frame > currentFrame)
        {
            skipFrames(frame-currentFrame);
            getFrame(data);
        }
        else
        {
            beginning();
            skipFrames(frame-1);
            getFrame(data);
        }
        return data;
    }

protected:

    std::filesystem::path path;
    std::ifstream filestream;
    uint64_t atoms;
    uint64_t frames;
    uint64_t currentFrame;
    uint64_t linesInFile;

    virtual void beginning()
    {
      filestream.seekg(std::ios::beg);
    }

    virtual void getFrame(std::vector<Atom> & data) = 0;

    virtual void skipFrame() = 0;

    virtual void skipFrames(uint64_t count) = 0;

    void skipLine()
    {
      filestream.ignore
      (
          std::numeric_limits<std::streamsize>::max(),
          '\n'
      );
    }

    void countLinesInFile()
    {
      filestream.seekg(std::ios::beg);
      linesInFile = std::count
      (
          std::istreambuf_iterator<char>
          {filestream},
          {},
          '\n'
      );
    }
};

#endif /* STRUCTURE_H */
