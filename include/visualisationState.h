#ifndef VISUALISATIONSTATE_H
#define VISUALISATIONSTATE_H

#include <vector>
#include <map>
#include <cstdint>
#include <string>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <lua.h>

#include <colour.h>
#include <bond.h>
#include <atom.h>
#include <element.h>
#include <LuaNumber.h>
#include <util.h>
#include <commandLine.h>

#include <record.h>

#ifdef WITH_FFMPEG
    #include <ffmpegRecord.h>
#else
    #include <jompegRecord.h>
#endif

/**
 * @brief Holds editable data for the visualisation state.
 *
 */
struct VisualisationState
{
    /**
     * @brief Construct a VisualisationState from a some Atoms.
     *
     * @param atoms the Atoms to construct from.
     * @param atomColours file path for colour overrides.
     * @param bondFocus a particular atom to focus bonds on.
     * @param bondCutoff a bond cutoff distance.
     * @param keyCodes key codes.
     */
    VisualisationState
    (
        std::vector<Atom> & atoms,
        const std::filesystem::path & atomColours,
        uint64_t bondFocus,
        float bondCutoff,
        bool sizeByMass,
        const std::map<int, std::string> & keyCodes
    )
    : atoms(atoms)
    {
        std::set<Element> elements = uniqueElements(atoms);
        elementMap = elementIndices(atoms);

        atomEmphasisOverrides = std::vector<float>(atoms.size(), 1.0f);

        for (uint8_t i = 0; i < std::min(size_t(6), elements.size()); i++)
        {
            Element e = *std::next(elements.begin(), i);
            emphasisControls[GLFW_KEY_1+i] = e;
            std::cout << "Element " << e << " emphasis bound to key " << keyCodes.at(GLFW_KEY_1+i) << "\n";
        }

        if (!atomColours.empty())
        {
            atomColourOverrides = atomColoursFromFile(atomColours);
        }

        applyColours(atoms, atomColourOverrides);

        if (bondFocus < atoms.size())
        {
            bondsFor = {bondFocus};
        }
        else
        {
            bondsFor.resize(atoms.size());
            std::iota(bondsFor.begin(), bondsFor.end(), 0);
        }

        if (bondCutoff > 0.0)
        {
            bonds = determineBonds
            (
                bondsFor,
                atoms,
                bondCutoff
            );
        }
        atomCount = atoms.size();

        atomSizes.resize(atoms.size());
        if (!sizeByMass)
        {
            for (const auto & ei : elementMap)
            {
                atomSizes[ei.second] = ELEMENT_RADIUS.at(ei.first);
            }
        }
        else
        {
            for (const auto & ei : elementMap)
            {
                atomSizes[ei.second] = ELEMENT_MASS.at(ei.first);
            }
        }
        applySizes(atoms, atomSizes);

        text = "";
        frame = 0;
    }

    std::vector<Atom> & atoms;
    std::map<uint64_t, std::set<uint64_t>> bonds;
    std::vector<uint64_t> bondsFor;
    std::vector<float> atomEmphasisOverrides;
    std::map<uint64_t, glm::vec4> atomColourOverrides;
    std::vector<float> atomSizes;
    std::multimap<Element, uint64_t> elementMap;
    std::map<int, Element> emphasisControls;
    std::string text;
    uint64_t frame;

    uint64_t atomCount;

    std::unique_ptr<Record> record = nullptr;

    bool recording = false;
    bool recordClosing = false;

    /**
     * @brief Video writing is behind.
     *
     * @return true the main process should wait.
     * @return false the main process may continue the trajectory.
     */
    bool recordWaiting() const { return waitingForRecord; }

    /**
     * @brief Toggle recording to video.
     *
     * @param options the command line options (with video options).
     */
    void toggleRecord(const CommandLine & options)
    {
        if (!recording)
        {
            std::string name = timeStamp()+std::string(".mp4");
            #ifdef WITH_FFMPEG
            record = std::make_unique<FFmpegRecord>
            (
                name,
                options.resolution.value,
                60,
                options.preset.value,
                options.profile.value,
                options.crf.value,
                options.cq.value,
                options.qp.value,
                options.codec.value,
                options.maxBFrames.value,
                options.gopSize.value
            );
            std::cout << "FFmpeg ";
            #else
            record = std::make_unique<JompegRecord>
            (
                name,
                options.resolution.value,
                60
            );
            std::cout << "jo_mpeg ";
            #endif
            std::cout << "recording to " + name + "\n";
            record->open();
            recording = true;
        }
        else if (recording)
        {
            if (record->finalise())
            {
                record.reset();
                recording = false;
            }
            else
            {
                recordClosing = true;
            }
        }
    }

    /**
     * @brief If recording, obtain the pixels for the current frame and submit for recording.
     *
     * @param resX the x resolution.
     * @param resY the y resolution.
     */
    void recordFrame
    (
        uint32_t resX,
        uint32_t resY
    )
    {
        if (recordClosing || record == nullptr || (!record->isOpen()))
        {
            return;
        }

        std::vector<uint8_t> pixels(resX*resY*4, 0);
        glReadPixels
        (
            0,
            0,
            resX,
            resY,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixels.data()
        );

        for(int j = 0; j < int(resY/2); j++)
        {
            std::swap_ranges
            (
                pixels.begin()+4*resX*j,
                pixels.begin()+4*resX*(j+1),
                pixels.begin()+4*resX*(resY-j-1)
            );
        }
        record->queueFrame(pixels);

        if (record->queueSize() >= 32)
        {
            record->writeFrames();
        }

        if (record->framesLeft() >= 64)
        {
            waitingForRecord = true;
        }
        else
        {
            waitingForRecord = false;
        }
    }

    /**
     * @brief Lua binding to set an Atom's colour by index.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * 2. The red channel [0, 1].
     * 3. The green channel [0, 1].
     * 4. The blue channel [0, 1].
     * 5. [Optional] The alpha channel [0, 1] defaults to 1.
     * Call as sfoav.setAtomColour(5, 1.0, 0.5, 0.5, 1.0)
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_setAtomColour(lua_State * lua);

    /**
     * @brief Lua binding to get a Atom's colour by index.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getAtomColour(lua_State * lua);

    /**
     * @brief Lua binding to bond 2 Atoms.
     *
     * @remark Lua arguments are:
     * 1. The first Atom index.
     * 2. The second Atom index.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_bond(lua_State * lua);

    /**
     * @brief Lua binding to unbond 2 Atoms.
     *
     * @remark Lua arguments are:
     * 1. The first Atom index.
     * 2. The second Atom index.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_unbond(lua_State * lua);

    /**
     * @brief Lua binding to get the bonds of an Atom.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getAtomsBonds(lua_State * lua);

    /**
     * @brief Lua binding to get the Atom count.
     *
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_atomCount(lua_State * lua);

    /**
     * @brief Lua binding to get the neighbours of an Atom to a cutoff.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * 2. The cutoff distance.
     * 3. [Optional] whether to use nearest images.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getAtomsNeighbours(lua_State * lua);

    /**
     * @brief Lua binding to get an Atom.
     *
     * @remark Lua arguments are:
     * 1. The Atom index.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getAtom(lua_State * lua);

    /**
     * @brief Set the on screen text.
     *
     * @remark Lua arguments are:
     * 1. The text string.
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_setText(lua_State * lua);

    /**
     * @brief Get the current frame number.
     *
     * @param lua the Lua context.
     * @return int the return code.
     */
    inline int lua_getFrame(lua_State * lua);

private:

    bool waitingForRecord = false;

};

#endif /* VISUALISATIONSTATE_H */

#include <luaBindings/visualisationState/atoms.h>
#include <luaBindings/visualisationState/bonds.h>
#include <luaBindings/visualisationState/utils.h>