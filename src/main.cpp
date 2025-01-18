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

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));
    camera.setPosition(0.0f, 0.0f);

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(options.msaa.value);

    if (options.structure.value.empty())
    {
        throw std::runtime_error("No atoms path specified, specify one with -atoms <path>");
    }

    XYZ xyz(options.structure.value);
    auto atoms = xyz.readFrame(0);
    for (auto & atom : atoms)
    {
        std::cout << atom << "\n";
    }
    center(atoms);
    std::vector<Bond> bonds = determineBonds(atoms, options.bondCutoff.value);

    glm::vec3 cameraPositionSpherical = glm::vec3(10.0f, 1.96f, M_PI);

    glm::mat4 projection = glm::perspective
    (
        glm::radians(45.0f), float(resX)/float(resY),
        0.1f,
        1000.0f
    );

    glm::mat4 view = glm::lookAt
    (
        spherical2cartesian(cameraPositionSpherical),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0)
    );

    AtomRenderer atomRenderer
    (
        atoms,
        options.levelOfDetail.value,
        spherical2cartesian(cameraPositionSpherical),
        options.mesh.value
    );

    BondRenderer bondRenderer
    (
        bonds,
        atoms,
        bonds.size()
    );

    std::cout << bonds.size() << "\n";

    atomRenderer.setProjection(projection);
    atomRenderer.setView(view);
    atomRenderer.setLighting
    (
        spherical2cartesian(cameraPositionSpherical),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

    bondRenderer.setProjection(projection);
    bondRenderer.setView(view);
    bondRenderer.setLighting
    (
        spherical2cartesian(cameraPositionSpherical),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

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

        if (display.keyHasEvent(GLFW_KEY_W, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_W, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.x -= dr;
        }
        if (display.keyHasEvent(GLFW_KEY_S, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_S, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.x += dr;
        }
        if (display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_Q, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.y += dtheta;
        }
        if (display.keyHasEvent(GLFW_KEY_E, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_E, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.y -= dtheta;
        }
        if (display.keyHasEvent(GLFW_KEY_A, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_A, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.z -= dphi;
            if ( cameraPositionSpherical.z < 0) { cameraPositionSpherical.z += 2.0*M_PI; }
            else if ( cameraPositionSpherical.z > 2.0*M_PI) { cameraPositionSpherical.z = std::fmod(cameraPositionSpherical.z, 2.0*M_PI); }
        }
        if (display.keyHasEvent(GLFW_KEY_D, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_D, jGL::EventType::HOLD))
        {
            cameraPositionSpherical.z += dphi;
            if ( cameraPositionSpherical.z < 0) { cameraPositionSpherical.z += 2.0*M_PI; }
            else if ( cameraPositionSpherical.z > 2.0*M_PI) { cameraPositionSpherical.z = std::fmod(cameraPositionSpherical.z, 2.0*M_PI); }
        }

        if (display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_LEFT, jGL::EventType::HOLD))
        {
            cameraPositionSpherical = cartesian2Spherical(spherical2cartesian(cameraPositionSpherical) - glm::vec3(dr, 0.0, 0.0));
        }
        if (display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::PRESS) || display.keyHasEvent(GLFW_KEY_RIGHT, jGL::EventType::HOLD))
        {
            cameraPositionSpherical = cartesian2Spherical(spherical2cartesian(cameraPositionSpherical) + glm::vec3(dr, 0.0, 0.0));
        }

        view = glm::lookAt
        (
            spherical2cartesian(cameraPositionSpherical),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0)
        );

        atomRenderer.setProjection(projection);
        atomRenderer.setView(view);
        atomRenderer.setLighting
        (
            spherical2cartesian(cameraPositionSpherical),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        bondRenderer.setProjection(projection);
        bondRenderer.setView(view);
        bondRenderer.setLighting
        (
            spherical2cartesian(cameraPositionSpherical),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        atomRenderer.updateAtoms(atoms);
        //atomRenderer.draw(!options.meshes.value);

        bondRenderer.draw();

        std::stringstream debugText;

        debugText << "Delta: " << fixedLengthNumber(delta,6) << " ms"
                  << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                  << ")\n"
                  << "Atoms/Triangles: " << atoms.size() << "/" << atomRenderer.triangles(true) << "\n"
                  << "Pos: " << cameraPositionSpherical;

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