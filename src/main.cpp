#include <main.h>

std::string Console::stackTrace("");

int main(int argv, char ** argc)
{
    CommandLine options(argv, argc);

    const uint16_t resX = options.resolution.value.x;
    const uint16_t resY = options.resolution.value.y;

    Theme theme = options.darkTheme.value ? darkTheme() : lightTheme();

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;

    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif
    conf.CLIP_TO_MONITOR = false;
    conf.CLIP_TO_WORK_AREA = false;
    jGL::DesktopDisplay display
    (
        glm::ivec2(resX, resY),
        "SimpleFastOpenAtomicVisualiser",
        keyEventCallback,
        jGL::defaultMouseButtonCallback,
        jGL::defaultScrollCallback,
        conf
    );

    display.setFrameLimit(60);
    std::vector<std::byte> vicon(icon.begin(), icon.end());
    display.setIcon({vicon});

    glewInit();

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(glm::ivec2(resX, resY)));

    jGL::OrthoCam jglCamera(resX, resY, glm::vec2(0.0,0.0));
    jglCamera.setPosition(0.0f, 0.0f);

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(options.msaa.value);

    double deltas[60];
    double delta = 0;
    unsigned frameId = 0;
    unsigned int rbo;
    bool readInProgress = false;
    bool elementsNeedUpdate = false;
    bool playBackward = false;
    uint8_t lastAutoPlayIncrement = 0;

    if (options.msaa.value > 0)
    {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, options.msaa.value, GL_DEPTH24_STENCIL8, resX, resY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    if (options.structure.value.empty())
    {
        throw std::runtime_error("No atoms path specified, specify one with -atoms <path>");
    }

    std::unique_ptr<Structure> structure;
    readStructureFile(options.structure.value, structure);
    glm::vec3 com = glm::vec3(0);

    if (!options.colourmap.value.empty())
    {
        structure->colourMap = coloursFromFile(options.colourmap.value);
    }

    structure->readFrame(0);

    Camera loadingCamera {sfoavAtoms, resX, resY, options.fieldOfView.value};
    loadingCamera.rotate(-M_PI/2.0);

    AtomRenderer loadingAtoms
    (
        sfoavAtoms,
        options.levelOfDetail.value,
        loadingCamera.position(),
        options.mesh.value,
        options.atomClipCorrection.value
    );
    loadingAtoms.setAtomScale(options.atomSize.value);

    while (display.isOpen() && !structure->frameReadComplete())
    {
        uint64_t frame = structure->framePosition();
        if (frame > 0) { frame -= 1; }
        else { frame = structure->frameCount()-1; }
        std::stringstream progress;
        progress << "Frame: " << frame+1 << "/" << structure->frameCount()
                 << "\nFrame cacheing " << (structure->framePositionsLoaded() ? "complete." : "in progress.")
                 << "\nRead atom " << structure->frameReadProgress() << "/" << structure->atomCount();
        loadingScreenFrame(display, loadingCamera, loadingAtoms, progress.str(), theme, resX, resY, options.hideInfoText.value);
    }

    if (!display.isOpen()) { return 0; }

    VisualisationState visualisationState
    (
        structure->atoms,
        options.atomColours.value,
        options.bondFocus.value,
        options.bondCutoff.value,
        options.sizeByMass.value,
        keyCodes
    );

    if (!options.noCentering.value) { center(structure->atoms); }
    if (options.focus.value < structure->atoms.size()) { centerOn(structure->atoms, options.focus.value); }
    com = getCenter(structure->atoms);

    Camera camera {structure->atoms, resX, resY, options.fieldOfView.value};

    jLog::Log log;
    Console console(log, &visualisationState, &options, &camera);

    AtomRenderer atomRenderer
    (
        structure->atoms,
        options.levelOfDetail.value,
        camera.position(),
        options.mesh.value,
        options.atomClipCorrection.value
    );
    atomRenderer.setAtomScale(options.atomSize.value);
    atomRenderer.setGlobalAlpha(options.globalAtomAlpha.value);

    BondRenderer bondRenderer
    (
        visualisationState.bonds,
        structure->atoms,
        visualisationState.bonds.size(),
        options.bondClipCorrection.value
    );

    bondRenderer.setBondScale(options.bondSize.value);
    bondRenderer.setGlobalAlpha(options.globalBondAlpha.value);

    Axes axes(camera);
    Cell cell
    (
        structure->getCellA(),
        structure->getCellB(),
        structure->getCellC()
    );

    elementsNeedUpdate = true;

    atomRenderer.updateCamera(camera);
    bondRenderer.updateCamera(camera);
    if (!options.noTransparencySorting.value)
    {
        setTransparencySorting(structure->atoms, atomRenderer, bondRenderer);
    }
    else
    {
        atomRenderer.setTransparencySorting(false);
        bondRenderer.setTransparencySorting(false);
    }

    jGLInstance->setClear(theme.background);

    while (display.isOpen())
    {
        auto tic = std::chrono::high_resolution_clock::now();

        jGLInstance->beginFrame();

        jGLInstance->clear();

        if (closing)
        {
            if (visualisationState.record != nullptr)
            {
                if (visualisationState.record->finalise())
                {
                    display.close();
                    break;
                }
            }
            else
            {
                display.close();
                break;
            }
        }

        if (display.keyHasEvent(GLFW_KEY_V, jGL::EventType::PRESS))
        {
            visualisationState.toggleRecord(options);
        }

        if (visualisationState.recordClosing && visualisationState.record->finalise())
        {
            visualisationState.record.reset();
            visualisationState.recording = false;
            visualisationState.recordClosing = false;
        }

        if (display.keyHasEvent(GLFW_KEY_H, jGL::EventType::PRESS))
        {
            options.hideAtoms.value = !options.hideAtoms.value;
        }

        if (display.keyHasEvent(GLFW_KEY_I, jGL::EventType::PRESS))
        {
            options.hideInfoText.value = !options.hideInfoText.value;
        }

        bool cameraMoved = cameraControls(display, camera);

        elementsNeedUpdate = atomControls
        (
            display,
            structure->atoms,
            visualisationState.emphasisControls,
            visualisationState.elementMap,
            visualisationState.atomEmphasisOverrides,
            options.deemphasisAlpha.value
        );

        if (display.keyHasAnyEvents(GLFW_KEY_SPACE, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
        {
            if (!options.noCentering.value) { center(structure->atoms); }
            if (options.focus.value < structure->atoms.size()) { centerOn(structure->atoms, options.focus.value); }
            camera.reset(structure->atoms);
            elementsNeedUpdate = true;
            cameraMoved = true;
        }

        if (display.keyHasAnyEvents(GLFW_KEY_F, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
        {
            playBackward = false;
            if (!readInProgress)
            {
                com = getCenter(structure->atoms);
                structure->readFrame(structure->framePosition());
                readInProgress = true;
            }
        }

        if (display.keyHasAnyEvents(GLFW_KEY_B, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
        {
            playBackward = true;
            if (!readInProgress)
            {
                com = getCenter(structure->atoms);
                backward(structure);
                readInProgress = true;
            }
        }

        if (display.keyHasEvent(GLFW_KEY_R, jGL::EventType::PRESS))
        {
            if (!readInProgress)
            {
                structure->readFrame(0);
                readInProgress = true;
            }
        }

        if (display.keyHasEvent(GLFW_KEY_X, jGL::EventType::PRESS))
        {
            options.showAxes.value = !options.showAxes.value;
        }

        if (display.keyHasEvent(GLFW_KEY_C, jGL::EventType::PRESS))
        {
            options.showCell.value = !options.showCell.value;
        }

        if (display.keyHasEvent(GLFW_KEY_P, jGL::EventType::PRESS))
        {
            options.play.value = !options.play.value;
        }

        if (display.keyHasAnyEvents(GLFW_KEY_K, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
        {
            options.speed.value = std::min(options.speed.value+1, 60);
        }

        if (display.keyHasAnyEvents(GLFW_KEY_J, {jGL::EventType::PRESS, jGL::EventType::HOLD}))
        {
            if (options.speed.value > 1)
            {
                options.speed.value--;
            }
        }

        if (readInProgress && structure->frameReadComplete())
        {
            // Previous threaded read is done.
            readInProgress = false;
            visualisationState.atoms = structure->atoms;
            if (!options.noCentering.value) { center(structure->atoms); }
            if (options.focus.value < structure->atoms.size()) { centerOn(structure->atoms, options.focus.value); }
            else { translate(structure->atoms, com); }
            if (options.bondCutoff.value > 0.0)
            {
                visualisationState.bonds = determineBonds(visualisationState.bondsFor, structure->atoms, options.bondCutoff.value);
            }
            setAlpha(structure->atoms, visualisationState.atomEmphasisOverrides);
            applyColours(structure->atoms, visualisationState.atomColourOverrides);
            applySizes(structure->atoms, visualisationState.atomSizes);
            cell.setVectors(structure->getCellA(), structure->getCellB(), structure->getCellC());
            elementsNeedUpdate = true;
        }

        if (cameraMoved)
        {
            atomRenderer.updateCamera(camera);
            bondRenderer.updateCamera(camera);
        }

        if (!readInProgress && std::filesystem::exists(options.script.value))
        {
            console.runFile(options.script.value.string());
            setAlpha(structure->atoms, visualisationState.atomEmphasisOverrides);
            applyColours(structure->atoms, visualisationState.atomColourOverrides);
            applySizes(structure->atoms, visualisationState.atomSizes);
            atomRenderer.updateCamera(camera);
            bondRenderer.updateCamera(camera);
            elementsNeedUpdate = true;
        }

        if (elementsNeedUpdate)
        {
            if (!options.hideAtoms.value)
            {
                atomRenderer.updateAtoms(structure->atoms);
            }

            bondRenderer.update(visualisationState.bonds, structure->atoms);
            if (!options.noTransparencySorting.value)
            {
                setTransparencySorting(structure->atoms, atomRenderer, bondRenderer);
            }
        }

        bondRenderer.draw();
        if (!options.hideAtoms.value)
        {
            atomRenderer.draw(!options.meshes.value);
        }


        visualisationState.frame = structure->framePosition();
        if (visualisationState.frame > 0) { visualisationState.frame -= 1; }
        else { visualisationState.frame = structure->frameCount()-1; }

        if (!options.hideInfoText.value)
        {
            auto cx = fixedLengthNumber(camera.position().x, 6);
            auto cy = fixedLengthNumber(camera.position().y, 6);
            auto cz = fixedLengthNumber(camera.position().z, 6);
            std::stringstream debugText;
            debugText << "Frame: " << visualisationState.frame+1 << "/" << structure->frameCount()
                      << "\nFrame cacheing " << (structure->framePositionsLoaded() ? "complete." : "in progress.")
                      << "\nCamera: " << cx << ", " << cy << ", " << cz
                      << "\nDelta: " << fixedLengthNumber(delta,6) << " ms"
                      << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                      << ")\n"
                      << "Atoms/Triangles: " << structure->atoms.size() << "/" << atomRenderer.triangles(true)+bondRenderer.triangles()
                      << "\nSpeed " << fixedLengthNumber(int(100.0*float(options.speed.value)/float(60)), 3);

            jGLInstance->text(
                debugText.str(),
                glm::vec2(64.0f, resY-64.0f),
                0.5f,
                theme.text
            );
        }
        else if (visualisationState.text != "")
        {
            jGLInstance->text(
                visualisationState.text,
                glm::vec2(64.0f, resY-64.0f),
                0.5f,
                theme.text
            );
        }

        if ((closing || visualisationState.recordClosing) && visualisationState.record != nullptr && visualisationState.record->isOpen())
        {
            jGLInstance->text(
                "Writing video frames: " + std::to_string(visualisationState.record->framesLeft()),
                glm::vec2(64.0f, 64.0f),
                0.5f,
                theme.text
            );
        }

        if (options.showAxes.value)
        {
            axes.updateCamera(camera);
            axes.draw();
        }

        if (options.showCell.value)
        {
            cell.setProjectionView(camera.getPV());
            cell.draw();
        }

        if (!closing && !readInProgress && options.play.value && !visualisationState.recordWaiting())
        {
            uint8_t t = frameId < lastAutoPlayIncrement ?
              uint8_t(60)-std::min(lastAutoPlayIncrement,uint8_t(60))+frameId :
              frameId-lastAutoPlayIncrement;

            if (t >= uint8_t(60)-std::min(options.speed.value, uint8_t(60)))
            {
                com = getCenter(structure->atoms);
                if (playBackward)
                {
                    backward(structure);
                }
                else
                {
                    structure->readFrame(structure->framePosition());
                }
                readInProgress = true;
                lastAutoPlayIncrement = frameId;
            }
        }

        jGLInstance->endFrame();

        if (!closing && elementsNeedUpdate)
        {
            visualisationState.recordFrame(resX, resY);
        }

        if (display.keyHasEvent(GLFW_KEY_G, jGL::EventType::PRESS))
        {
            screenshot(options.resolution.value);
        }

        display.loop();

        if (console.exitCalled()) { closing = true; }

        delta = 0.0;
        for (int n = 0; n < 60; n++)
        {
            delta += deltas[n];
        }
        delta /= 60.0;
        auto toc = std::chrono::high_resolution_clock::now();
        deltas[frameId] = std::chrono::duration_cast<std::chrono::milliseconds>(toc-tic).count();
        frameId = (frameId+1) % 60;
        if (frameId == 0 && log.size() > 0) { std::cout << log << "\n"; }
    }

    if (options.msaa.value > 0)
    {
        glDeleteRenderbuffers(1, &rbo);
    }
    jGLInstance->finish();

    return 0;
}