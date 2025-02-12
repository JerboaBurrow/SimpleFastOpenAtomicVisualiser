#include <main.h>
int main(int argv, char ** argc)
{
    CommandLine options(argv, argc);

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;

    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif
    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "SimpleFastOpenAtomicVisualiser", conf);
    display.setFrameLimit(60);
    std::vector<std::byte> vicon(icon.begin(), icon.end());
    display.setIcon({vicon});

    glewInit();

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(glm::ivec2(resX,resY)));

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

    Camera loadingCamera {sfoavAtoms, resX, resY};
    loadingCamera.rotate(-M_PI/2.0);

    AtomRenderer loadingAtoms
    (
        sfoavAtoms,
        options.levelOfDetail.value,
        loadingCamera.position(),
        options.mesh.value
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
        loadingScreenFrame(display, loadingCamera, loadingAtoms, progress.str());
    }

    if (!display.isOpen()) { return 0; }

    std::set<Element> elements = uniqueElements(structure->atoms);
    std::multimap<Element, uint64_t> elementMap = elementIndices(structure->atoms);
    std::map<int, Element> emphasisControls;
    std::vector<float> alphaOverrides(structure->atoms.size(), 1.0f);
    for (uint8_t i = 0; i < std::min(size_t(6), elements.size()); i++)
    {
        Element e = *std::next(elements.begin(), i);
        emphasisControls[GLFW_KEY_1+i] = e;
        std::cout << "Element " << e << " emphasis bound to key " << keyCodes.at(GLFW_KEY_1+i) << "\n";
    }

    center(structure->atoms);

    std::vector<Bond> bonds;
    if (options.bondCutoff.value > 0.0)
    {
        bonds = determineBonds(structure->atoms, options.bondCutoff.value);
    }

    Camera camera {structure->atoms, resX, resY};

    AtomRenderer atomRenderer
    (
        structure->atoms,
        options.levelOfDetail.value,
        camera.position(),
        options.mesh.value
    );
    atomRenderer.setAtomScale(options.atomSize.value);

    BondRenderer bondRenderer
    (
        bonds,
        structure->atoms,
        bonds.size()
    );

    bondRenderer.setBondScale(options.bondSize.value);

    Axes axes(camera);
    Cell cell
    (
        structure->getCellA(),
        structure->getCellB(),
        structure->getCellC()
    );

    elementsNeedUpdate = true;

    while (display.isOpen())
    {
        auto tic = std::chrono::high_resolution_clock::now();

        jGLInstance->beginFrame();
        jGLInstance->setClear(glm::vec4(1.0f));
        jGLInstance->clear();

        if (display.keyHasEvent(GLFW_KEY_H, jGL::EventType::PRESS))
        {
            options.hideAtoms.value = !options.hideAtoms.value;
        }

        cameraControls(display, camera);
        elementsNeedUpdate = atomControls
        (
            display,
            structure->atoms,
            emphasisControls,
            elementMap,
            alphaOverrides,
            options.deemphasisAlpha.value
        );

        if (display.keyHasEvent(GLFW_KEY_SPACE, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_SPACE, jGL::EventType::HOLD))
        {
            center(structure->atoms);
            camera.reset(structure->atoms);
            elementsNeedUpdate = true;
        }

        if (display.keyHasEvent(GLFW_KEY_F, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_F, jGL::EventType::HOLD))
        {
            if (!readInProgress)
            {
                com = getCenter(structure->atoms);
                structure->readFrame(structure->framePosition());
                readInProgress = true;
            }
        }
        if (display.keyHasEvent(GLFW_KEY_B, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_B, jGL::EventType::HOLD))
        {
            if (!readInProgress)
            {
                com = getCenter(structure->atoms);
                uint64_t f = structure->framePosition();
                if (f > 2) { f -= 2; }
                else { f = structure->frameCount()-2+f;}
                structure->readFrame(f);
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

        if (readInProgress && structure->frameReadComplete())
        {
            // Previous threaded read is done.
            readInProgress = false;
            center(structure->atoms);
            translate(structure->atoms, com);
            if (options.bondCutoff.value > 0.0)
            {
                bonds = determineBonds(structure->atoms, options.bondCutoff.value);
            }
            setAlpha(structure->atoms, alphaOverrides);
            cell.setVectors(structure->getCellA(), structure->getCellB(), structure->getCellC());
            elementsNeedUpdate = true;
        }

        atomRenderer.updateCamera(camera);
        bondRenderer.updateCamera(camera);

        if (!options.hideAtoms.value)
        {
            if (elementsNeedUpdate) { atomRenderer.updateAtoms(structure->atoms); }
            atomRenderer.draw(!options.meshes.value);
        }

        if (elementsNeedUpdate) { bondRenderer.update(bonds, structure->atoms); }
        bondRenderer.draw();

        elementsNeedUpdate = false;

        std::stringstream debugText;

        uint64_t frame = structure->framePosition();
        if (frame > 0) { frame -= 1; }
        else { frame = structure->frameCount()-1; }

        auto cx = fixedLengthNumber(camera.position().x, 6);
        auto cy = fixedLengthNumber(camera.position().y, 6);
        auto cz = fixedLengthNumber(camera.position().z, 6);

        debugText << "Delta: " << fixedLengthNumber(delta,6) << " ms"
                  << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                  << ")\n"
                  << "Atoms/Triangles: " << structure->atoms.size() << "/" << atomRenderer.triangles(true)+bondRenderer.triangles() << "\n"
                  << "Frame: " << frame+1 << "/" << structure->frameCount()
                  << "\nFrame cacheing " << (structure->framePositionsLoaded() ? "complete." : "in progress.")
                  << "\nCamera: " << cx << ", " << cy << ", " << cz;

        jGLInstance->text(
            debugText.str(),
            glm::vec2(64.0f, resY-64.0f),
            0.5f,
            glm::vec4(0.0f,0.0f,0.0f,1.0f)
        );

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

        jGLInstance->endFrame();
        display.loop();

        delta = 0.0;
        for (int n = 0; n < 60; n++)
        {
            delta += deltas[n];
        }
        delta /= 60.0;
        auto toc = std::chrono::high_resolution_clock::now();
        deltas[frameId] = std::chrono::duration_cast<std::chrono::milliseconds>(toc-tic).count();
        frameId = (frameId+1) % 60;
    }

    if (options.msaa.value > 0)
    {
        glDeleteRenderbuffers(1, &rbo);
    }
    jGLInstance->finish();

    return 0;
}