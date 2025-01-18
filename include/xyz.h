#ifndef XYZ_H
#define XYZ_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <structure.h>

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
class XYZ : public Structure
{
public:

    /**
     * @brief Construct a new XYZ object to read from path.
     *
     * @param path the file path of the XYZ file.
     */
    XYZ(std::filesystem::path path)
    : Structure(path)
    {
        readAtomCount();
        readFrameCount();
    }

private:

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
        frames = linesInFile / (atoms+uint64_t(2));
    }

    void skipFrame()
    {
        for (uint64_t a = 0; a < atoms+2; a++)
        {
            skipLine();
        }
    }

    void skipFrames(uint64_t count)
    {
        for (uint64_t f = 0; f < count; f++)
        {
            skipFrame();
        }
    }

    void getFrame(std::vector<Atom> & data)
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
