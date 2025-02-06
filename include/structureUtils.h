#ifndef STRUCTUREUTILS_H
#define STRUCTUREUTILS_H

#include <memory>

#include <xyz.h>
#include <config.h>

/**
 * @brief Read a structure file from the path.
 *
 * @remark Will attemp to automatically detect CONFIG-like of [EXT]XYZ files.
 * @remark Will try both on failure.
 * @param path the structure file's path.
 * @param structure the structure unique pointer.
 * @param blocking whether reads are blocking or detached.
 */
void readStructureFile
(
    std::filesystem::path path,
    std::unique_ptr<Structure> & structure,
    bool blocking = false
)
{
    if (!ostensiblyXYZLike(path))
    {
        if (!ostensiblyCONFIGLike(path))
        {
            std::cout << path << " does not appear to refer to an [EXT]XYZ or CONFIG-like\n";
        }
        try
        {
            structure = std::make_unique<CONFIG>(path, blocking);
        }
        catch (std::runtime_error & e)
        {
            std::cout << "Could not parse "
                      << path
                      << " as a CONFIG-like:\n"
                      << e.what() << "\n Trying [EXT]XYZ\n";
            structure = std::make_unique<XYZ>(path, blocking);
        }
    }
    else
    {
        try
        {
            structure = std::make_unique<XYZ>(path, blocking);
        }
        catch (std::runtime_error & e)
        {
            std::cout << "Could not parse "
                      << path
                      << " as an [EXT]XYZ:\n"
                      << e.what() << "\n Trying CONFIG-like\n";
            structure = std::make_unique<CONFIG>(path, blocking);
        }
    }
}

#endif /* STRUCTUREUTILS_H */
