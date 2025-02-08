#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>

#include <structure.h>

/**
 * @brief Check if a path is CONFIG'y
 *
 * @param path the path to check.
 * @return true if
 *  - The path stem is CONFIG, REVCON, CFGMIN, or HISTORY.
 *  - The extension is one of the above.
 *  - The above cases any type-case.
 * @return false false otherwise.
 */
bool ostensiblyCONFIGLike(std::filesystem::path path)
{
    const std::array<std::string, 4> names {"config", "revcon", "cfgmin", "history"};
    std::string ext = path.extension().string();
    std::string stem = path.stem().string();

    for (std::string * s : {&ext, &stem})
    {
        std::transform
        (
            s->begin(),
            s->end(),
            s->begin(),
            [](unsigned char c){ return std::tolower(c); }
        );
    }

    for (auto name : names)
    {
        if (ext.rfind("."+name, 0) == 0)
        {
            return true;
        }
        if (stem.rfind(name, 0) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Read CONFIG files
 * @remark The file structure is:
 * - A title line [string].
 * - A meta data line.
 *   - levcfg [integer] 0 = positions, 1 = positions and velocities, 2 = positions, velocities, and forces.
 *   - imcon [integer] boundary type.
 *   - megatm [integer] atom count.
 * - A set of n*(1+levcfg) recrods.
 *   - positions [float, float, float]
 *   - velocities [float, float, float], if levcfg > 0
 *   - forces [float, float, float], if levcfg > 1
 * @remark A trajectory is the HISTORY file which contains more information.
 */
class CONFIG : public Structure
{
public:

    /**
     * @brief Construct a new CONFIG object to read from path.
     *
     * @param path the file path of the CONFIG file.
     * @param blocking if reads are blocking or detached.
     */
    CONFIG(std::filesystem::path path, bool blocking = false)
    : Structure(path, blocking)
    {
        // Check first frame/meta data format.
        initialise();
        scanPositions();
    }

private:

    bool HISTORY = false;
    uint64_t metaDataLines;
    uint64_t linesPerAtom;
    unsigned levcfg;
    unsigned imcon;

    void initialise()
    {
        filestream.seekg(std::ios::beg);
        std::string line;
        skipLine(filestream);
        std::getline(filestream, line);
        std::stringstream data(line);
        timeStep = 0;
        data >> levcfg >> imcon >> natoms;
        if (data.fail())
        {
            std::stringstream message;
            message << path.c_str()
                    << " does not have an atom count.\n"
                    << "Please add the atom count after imcon.\n";
            throw std::runtime_error
            (
                message.str()
            );
        }
        else
        {
            // Could be a HISTORY or REVCON file.
            data = std::stringstream(line);
            data >> levcfg >> imcon >> natoms >> frames;
            if (extractHistoryStepMetaData())
            {
                HISTORY = true;
            }
        }

        if (HISTORY){ getCell(); }
        else
        {
            filestream.seekg(std::ios::beg);
            skipLine(filestream);
            skipLine(filestream);
            getCell();
        }

        if (!HISTORY) { metaDataLines = 2+(imcon != 0 ? 3 : 0); }
        else { metaDataLines = 2; }
        linesPerAtom = 2+(levcfg > 0 ? 1 : 0)+(levcfg > 1 ? 1 : 0);
        linesPerFrame = natoms*linesPerAtom+4;
        beginning();
        framePositions[0] = filestream.tellg();
        frames = 1;
        atoms.resize(natoms);
    }

    void beginning()
    {
        filestream.seekg(std::ios::beg);
        for (uint64_t l = 0; l < metaDataLines; l++)
        {
            skipLine(filestream);
        }
    }

    void getAtoms()
    {
        std::string line;
        std::stringstream ss;
        if (HISTORY)
        {
            skipLine(filestream);
            getCell();
        }
        for (uint64_t a = 0; a < atoms.size(); a++)
        {
            std::string symbol;
            uint64_t index;
            Atom atom;

            std::getline(filestream, line);
            ss = std::stringstream(line);
            ss >> symbol >> index;
            checkRead(ss, line, "CONFIG reading atom "+std::to_string(a));

            std::getline(filestream, line);
            ss = std::stringstream(line);
            ss  >> atom.position.x
                >> atom.position.y
                >> atom.position.z;
            checkRead(ss, line, "CONFIG reading atom "+std::to_string(a));

            if (levcfg > 0)
            {
                std::getline(filestream, line);
                ss = std::stringstream(line);
                ss  >> atom.velocity.x
                    >> atom.velocity.y
                    >> atom.velocity.z;
                checkRead(ss, line, "CONFIG reading atom "+std::to_string(a));
            }
            if (levcfg > 1)
            {
                std::getline(filestream, line);
                ss = std::stringstream(line);
                ss  >> atom.force.x
                    >> atom.force.y
                    >> atom.force.z;
                checkRead(ss, line, "CONFIG reading atom "+std::to_string(a));
            }

            atom.symbol = stringSymbolToElement(symbol);
            atom.scale = ELEMENT_RADIUS.at(atom.symbol);
            atom.colour = colourMap.at(atom.symbol);
            atoms[a] = atom;
            atomsRead = a+1;
        }
    }

    void getFrame()
    {
        atomsRead = 0;
        if (blockingReads) { getAtoms(); return; }
        std::thread io = std::thread
        (
            &CONFIG::getAtoms,
            this
        );
        io.detach();
    }

    void getCell()
    {
        std::string line;
        std::stringstream data;
        std::getline(filestream, line);
        data = std::stringstream(line);
        data >> cellA.x >> cellA.y >> cellA.z;
        checkRead(data, line, "getCell");

        std::getline(filestream, line);
        data = std::stringstream(line);
        data >> cellB.x >> cellB.y >> cellB.z;
        checkRead(data, line, "getCell");

        std::getline(filestream, line);
        data = std::stringstream(line);
        data >> cellC.x >> cellC.y >> cellC.z;
        checkRead(data, line, "getCell");
    }

    bool extractHistoryStepMetaData()
    {
        std::string line;
        std::stringstream data;
        std::getline(filestream, line);
        if (line.rfind("timestep", 0) == 0)
        {
            data = std::stringstream(line.substr(9));
            data >> timeStep;
            return true;
        }
        return false;
    }
};
#endif /* CONFIG_H */
