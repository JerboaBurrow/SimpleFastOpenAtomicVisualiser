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
    std::string ext = path.extension().string();
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
     * @param blocking if reads are blocking or detached.
     */
    XYZ(std::filesystem::path path, bool blocking = false)
    : Structure(path, blocking)
    {
        // Check first frame/meta data format.
        initialise();
        scanPositions();
    }

private:

    void initialise()
    {
        beginning();
        std::string line;
        std::getline(filestream, line);
        std::stringstream count(line);
        count >> natoms;
        checkRead(count, line, "XYZ readAtomCount");
        beginning();
        framePositions[0] = filestream.tellg();
        frames = 1;
        linesPerFrame = natoms+2;
        atoms.resize(natoms);
    }

    void getAtoms()
    {
        atomsRead = 0;
        std::string line;
        std::stringstream ss;
        std::getline(filestream, line);
        std::getline(filestream, line);
        for (uint64_t a = 0; a < atoms.size(); a++)
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
            atoms[a] = atom;
            atomsRead = a+1;
        }
    }

    void getFrame()
    {
        if (blockingReads) { getAtoms(); return; }
        std::thread io = std::thread
        (
            &XYZ::getAtoms,
            this
        );
        io.detach();
    }
};

#endif /* XYZ_H */
