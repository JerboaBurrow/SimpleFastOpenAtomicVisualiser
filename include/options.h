#ifndef OPTIONS_H
#define OPTIONS_H

#include <cstdint>

#include <hierarchicalTriangularMesh.h>
#include <constants.h>

struct Options
{
    /**
     * @brief Mesh level of detail.
     *
     */
    uint8_t levelOfDetail;

    /**
     * @brief MSAA level.
     *
     */
    const uint8_t msaa;

    bool meshes;
    BASE_MESH mesh;

    std::filesystem::path structure;
    
    float bondCutoff;
    float bondSize;

    bool hideAtoms;
    bool showAxes;
    bool showCell;

    float deemphasisAlpha;

    std::filesystem::path colourmap;
    std::filesystem::path atomColours;

    float atomSize;
    glm::ivec2 resolution;
    bool hideInfoText;
    bool play;
    uint64_t bond
};

#endif /* OPTIONS_H */
