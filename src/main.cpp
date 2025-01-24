#include <main.h>
int main(int argv, char ** argc)
{
    CommandLine options(argv, argc);

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;

    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif
    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "SimpleFastAtomicVisualiser", conf);
    display.setFrameLimit(60);
    std::vector<std::byte> vicon(icon.begin(), icon.end());
    display.setIcon({vicon});

    glewInit();

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(glm::ivec2(resX,resY)));

    jGL::OrthoCam jglCamera(resX, resY, glm::vec2(0.0,0.0));
    jglCamera.setPosition(0.0f, 0.0f);

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(options.msaa.value);

    if (options.structure.value.empty())
    {
        throw std::runtime_error("No atoms path specified, specify one with -atoms <path>");
    }

    std::vector<Atom> atoms;

    if (ostensiblyXYZLike(options.structure.value))
    {
        XYZ structure(options.structure.value);
        atoms = structure.readFrame(0);
    }
    else
    {
        CONFIG structure(options.structure.value);
        atoms = structure.readFrame(0);
    }

    center(atoms);

    Camera camera {atoms, resX, resY};

    std::vector<Bond> bonds = determineBonds(atoms, options.bondCutoff.value);

    AtomRenderer atomRenderer
    (
        atoms,
        options.levelOfDetail.value,
        camera.position(),
        options.mesh.value
    );

    BondRenderer bondRenderer
    (
        bonds,
        atoms,
        bonds.size()
    );

    atomRenderer.setProjection(camera.getProjection());
    atomRenderer.setView(camera.getView());
    atomRenderer.setLighting
    (
        camera.position(),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

    bondRenderer.setProjection(camera.getProjection());
    bondRenderer.setView(camera.getView());
    bondRenderer.setLighting
    (
        camera.position(),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

    bondRenderer.setBondScale(options.bondSize.value);

    double deltas[60];
    double delta = 0;
    unsigned frameId = 0;
    unsigned int rbo;

    if (options.msaa.value > 0)
    {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, options.msaa.value, GL_DEPTH24_STENCIL8, resX, resY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

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

        if (display.keyHasEvent(GLFW_KEY_W, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_W, jGL::EventType::HOLD))
        {
            camera.zoom(-dr);
        }
        if (display.keyHasEvent(GLFW_KEY_S, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_S, jGL::EventType::HOLD))
        {
            camera.zoom(dr);
        }
        if (display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::HOLD))
        {
            camera.incline(dtheta);
        }
        if (display.keyHasEvent(GLFW_KEY_E, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_E, jGL::EventType::HOLD))
        {
            camera.incline(-dtheta);
        }
        if (display.keyHasEvent(GLFW_KEY_A, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_A, jGL::EventType::HOLD))
        {
            camera.rotate(-dphi);
        }
        if (display.keyHasEvent(GLFW_KEY_D, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_D, jGL::EventType::HOLD))
        {
            camera.rotate(dphi);
        }

        if (display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::HOLD))
        {
            translate(atoms, {-dr, 0.0, 0.0});
        }
        if (display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::HOLD))
        {
            translate(atoms, {dr, 0.0, 0.0});
        }
        if (display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_PERIOD, jGL::EventType::HOLD))
        {
            translate(atoms, {0.0, -dr, 0.0});
        }
        if (display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_SLASH, jGL::EventType::HOLD))
        {
            translate(atoms, {0.0, dr, 0.0});
        }
        if (display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_DOWN, jGL::EventType::HOLD))
        {
            translate(atoms, {0.0, 0.0, -dr});
        }
        if (display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_UP, jGL::EventType::HOLD))
        {
            translate(atoms, {0.0, 0.0, dr});
        }

        if (display.keyHasEvent(GLFW_KEY_SPACE, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_SPACE, jGL::EventType::HOLD))
        {
            center(atoms);
            camera.reset(atoms);
        }

        atomRenderer.setProjection(camera.getProjection());
        atomRenderer.setView(camera.getView());
        atomRenderer.setLighting
        (
            camera.position(),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        bondRenderer.setProjection(camera.getProjection());
        bondRenderer.setView(camera.getView());
        bondRenderer.setLighting
        (
            camera.position(),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        if (!options.hideAtoms.value)
        {
            atomRenderer.updateAtoms(atoms);
            atomRenderer.draw(!options.meshes.value);
        }

        bondRenderer.draw();

        std::stringstream debugText;

        debugText << "Delta: " << fixedLengthNumber(delta,6) << " ms"
                  << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                  << ")\n"
                  << "Atoms/Triangles: " << atoms.size() << "/" << atomRenderer.triangles(true)+bondRenderer.triangles() << "\n";

        jGLInstance->text(
            debugText.str(),
            glm::vec2(64.0f, resY-64.0f),
            0.5f,
            glm::vec4(0.0f,0.0f,0.0f,1.0f)
        );

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