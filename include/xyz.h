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
 * @brief Check if a path is XYZ'y
 *
 * @param path the path to check.
 * @return true if the path ends with "xyz" or "extxzy" in any case.
 * @return false false otherwise.
 */
bool ostensiblyXYZLike(std::filesystem::path path)
{
    std::string ext = path.extension();
    std::transform
    (
        ext.begin(),
        ext.end(),
        ext.begin(),
        [](unsigned char c){ return std::tolower(c); }
    );
    if (ext.rfind(".xyz", 0) == 0 || ext.rfind(".extxyz", 0) == 0)
    {
        return true;
    }
    return false;
}

/**
 * @brief Read XYZ and EXTXYZ files
 * @remark The file structure is nLine reads: ++line2 lines for nLine reads:  +lineatoms.
 * - Atom count [integer]
 * - Comment line [string]
 * - nLine reads:  +lineentries of the form
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
        checkRead(count, line, "XYZ readAtomCount");
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
            checkRead(ss, line, "XYZ reading atom "+std::to_string(a));
            atom.symbol = stringSymbolToElement(symbol);
            atom.scale = ELEMENT_RADIUS.at(atom.symbol);
            atom.colour = CPK_COLOURS.at(atom.symbol);
            data.push_back(atom);
        }
        currentFrame++;
    }
};

#endif /* XYZ_H */
