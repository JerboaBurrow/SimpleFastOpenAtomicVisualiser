#ifndef XYZ_H
#define XYZ_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <atom.h>

/**
 * @brief Read XYZ and EXTXYZ files
 * @remark The file structure is n+2 lines for n atoms.
 * - Atom count [integer]
 * - Comment line [string]
 * - n entries of the form
 *   - Symbol [string]
 *   - Position [float, float, float]
 * @remark A trajectory is a simple concatenation of multiple XYZ files.
 * @remark EXTXYZ includes a more detail specification for the comment line.
 */
class XYZ
{
public:

    /**
     * @brief Construct a new XYZ object to read from path.
     *
     * @param path the file path of the XYZ file.
     */
    XYZ(std::filesystem::path path)
    : path(path),
      filestream(std::ifstream(path)),
      currentFrame(0)
    {
        readAtomCount();
        readFrameCount();
    }

    /**
     * @brief Get the number of atoms in the XYZ.
     *
     * @return uint64_t the number of atoms.
     */
    uint64_t atomCount() const { return atoms; }

    /**
     * @brief Read a single frame at position frame.
     *
     * @param frame the frame position.
     * @return std::vector<Atom> the Atoms read.
     */
    std::vector<Atom> readFrame(uint64_t frame)
    {
        std::vector<Atom> data;
        if (frame > frames) { return data; }
        if (frame == currentFrame)
        {
            readFrame(data);
        }
        else if (frame > currentFrame)
        {
            skipFrames(frame-currentFrame);
            readFrame(data);
        }
        else
        {
            filestream.seekg(std::ios::beg);
            skipFrames(frame-1);
            readFrame(data);
        }
        return data;
    }

private:

    std::filesystem::path path;
    std::ifstream filestream;
    uint64_t atoms;
    uint64_t frames;
    uint64_t currentFrame;

    void readAtomCount()
    {
        filestream.seekg(std::ios::beg);
        std::string line;
        std::getline(filestream, line);
        std::stringstream count(line);
        count >> atoms;
        filestream.seekg(std::ios::beg);
    }

    void readFrameCount()
    {
        filestream.seekg(std::ios::beg);
        uint64_t lines = std::count
        (
            std::istreambuf_iterator<char>
            {filestream},
            {},
            '\n'
        );
        frames = lines / (atoms+uint64_t(2));
        filestream.seekg(std::ios::beg);
    }

    void skipFrame()
    {
        filestream.ignore
        (
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
    }

    void skipFrames(uint64_t count)
    {
        for (uint64_t f = 0; f < count; f++)
        {
            skipFrame();
        }
    }

    void readFrame(std::vector<Atom> & data)
    {
        std::string line;
        std::stringstream ss;
        std::getline(filestream, line);
        std::getline(filestream, line);
        for (uint64_t a = 0; a < atoms; a++)
        {
            std::getline(filestream, line);
            std::string symbol;
            Atom atom;
            ss = std::stringstream(line);
            ss >> symbol
                >> atom.position.x
                >> atom.position.y
                >> atom.position.z;
            atom.symbol = stringSymbolToElement(symbol);
            atom.scale = ELEMENT_RADIUS.at(atom.symbol);
            atom.colour = CPK_COLOURS.at(atom.symbol);
            data.push_back(atom);
        }
        currentFrame++;
    }
};

#endif /* XYZ_H */
