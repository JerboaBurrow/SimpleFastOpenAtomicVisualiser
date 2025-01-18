#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <structure.h>

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
     */
    CONFIG(std::filesystem::path path)
    : Structure(path)
    {
        readMetaData();
    }

private:

    bool HISTORY = false;
    uint64_t metaDataLines;
    uint64_t linesPerAtom;
    unsigned levcfg;
    unsigned imcon;

    glm::vec3 cellA;
    glm::vec3 cellB;
    glm::vec3 cellC;

    void readMetaData()
    {
        filestream.seekg(std::ios::beg);
        std::string line;
        skipLine();
        std::getline(filestream, line);
        std::stringstream data(line);
        // HISTORY requires checking megatm.
        frames = 1;
        data >> levcfg >> imcon >> atoms;
        if (data.fail())
        {
            // Old CONFIGs had optional megatm.
            atoms = (linesInFile-2) / (levcfg+1);
        }
        else
        {
            // Could be a HISTORY file.
            data = std::stringstream(line);
            uint8_t records;
            data >> levcfg >> imcon >> atoms >> frames >> records;
            if (!data.fail())
            {
                HISTORY = true;
                // First frame meta data.
                skipLine();
            }
        }

        if (HISTORY || (!HISTORY && imcon != 0))
        {
            std::getline(filestream, line);
            data = std::stringstream(line);
            data >> cellA.x >> cellA.y >> cellA.z;

            std::getline(filestream, line);
            data = std::stringstream(line);
            data >> cellB.x >> cellB.y >> cellB.z;

            std::getline(filestream, line);
            data = std::stringstream(line);
            data >> cellC.x >> cellC.y >> cellC.z;
        }

        if (!HISTORY) { metaDataLines = 2+(imcon != 0 ? 3 : 0); }
        else { metaDataLines = 2; }
        linesPerAtom = 2+(levcfg > 0 ? 1 : 0)+(levcfg > 1 ? 1 : 0);
        beginning();
    }

    void skipFrame()
    {
        if (!HISTORY) { return; }
        for (uint64_t a = 0; a < atoms*linesPerAtom+4; a++)
        {
            skipLine();
        }
    }

    void skipFrames(uint64_t count)
    {
        if (!HISTORY) { return; }
        for (uint64_t f = 0; f < count; f++)
        {
            skipFrame();
        }
    }

    void beginning()
    {
        filestream.seekg(std::ios::beg);
        for (uint64_t l = 0; l < metaDataLines; l++)
        {
            skipLine();
        }
    }

    void getFrame(std::vector<Atom> & data)
    {
        std::string line;
        std::stringstream ss;
        for (uint64_t a = 0; a < atoms; a++)
        {
            std::string symbol;
            uint64_t index;
            Atom atom;

            std::getline(filestream, line);
            ss = std::stringstream(line);
            ss >> symbol >> index;

            std::getline(filestream, line);
            ss = std::stringstream(line);
            ss  >> atom.position.x
                >> atom.position.y
                >> atom.position.z;

            if (levcfg > 0)
            {
                std::getline(filestream, line);
                ss = std::stringstream(line);
                ss  >> atom.velocity.x
                    >> atom.velocity.y
                    >> atom.velocity.z;
            }
            if (levcfg > 1)
            {
                std::getline(filestream, line);
                ss = std::stringstream(line);
                ss  >> atom.force.x
                    >> atom.force.y
                    >> atom.force.z;
            }

            atom.symbol = stringSymbolToElement(symbol);
            atom.scale = ELEMENT_RADIUS.at(atom.symbol);
            atom.colour = CPK_COLOURS.at(atom.symbol);
            data.push_back(atom);
        }
        currentFrame++;
    }
};
#endif /* CONFIG_H */
