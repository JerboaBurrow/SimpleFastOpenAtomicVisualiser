#include <models.h>

int main(int argv, char ** argc)
{
    jGL::DesktopDisplay::Config conf;

    uint8_t MSAA = 0;
    if (argv > 1) { MSAA = std::stoi(argc[1]); }

    conf.VULKAN = false;

    #ifdef MACOS
    conf.COCOA_RETINA = true;
    #endif
    jGL::DesktopDisplay display(glm::ivec2(resX, resY), "SimpleFastAtomicVisualiser - Model test", conf);
    display.setFrameLimit(60);
    std::vector<std::byte> vicon(icon.begin(), icon.end());
    display.setIcon({vicon});

    glewInit();

    std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    std::string device = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    jGLInstance = std::move(std::make_unique<jGL::GL::OpenGLInstance>(glm::ivec2(resX,resY)));

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));
    camera.setPosition(0.0f, 0.0f);

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(MSAA);

    float d = 1.75f;

    std::vector<Atom> atoms;
    std::vector<Atom> impostorAtoms;
    int col = 0;
    int row = 0;
    for (int i = 0; i < count; i++)
    {
        atoms.push_back({{d*row, d*col, 0.0}, 1.0f});
        impostorAtoms.push_back({{d*row, d*col, 0.0}, 1.0f});
        row++;
        if (i % cols == 0) { col++; row = 0; }
    }

    center(atoms);
    center(impostorAtoms);
    for (int i = 0; i < count; i++)
    {
        atoms[i].position -= vec3<float>(-d*cols*0.75,0.0,0.0);
        impostorAtoms[i].position += vec3<float>(-d*cols*0.75,0.0,0.0);
    }

    glm::vec3 cameraPositionSpherical = glm::vec3(1*count, M_PI*0.5f, M_PI);

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

    AtomRenderer renderer(atoms, 8, spherical2cartesian(cameraPositionSpherical));
    AtomRenderer impostorRenderer(impostorAtoms, 0, spherical2cartesian(cameraPositionSpherical));
    renderer.setView(view);
    renderer.setProjection(projection);
    renderer.setLighting
    (
        spherical2cartesian(cameraPositionSpherical),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

    impostorRenderer.setView(view);
    impostorRenderer.setProjection(projection);
    impostorRenderer.setLighting
    (
        spherical2cartesian(cameraPositionSpherical),
        {1.0f, 1.0f, 1.0f},
        0.1f
    );

    double deltas[60];
    double delta = 0;
    unsigned frameId = 0;
    unsigned int rbo;

    if (MSAA > 0)
    {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH24_STENCIL8, resX, resY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    while (display.isOpen())
    {
        auto tic = std::chrono::high_resolution_clock::now();

        jGLInstance->beginFrame();
        jGLInstance->setClear(glm::vec4(1.0f));
        jGLInstance->clear();

        cameraPositionSpherical.z += dphi/20.0;

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

        renderer.setView(view);
        renderer.setLighting
        (
            spherical2cartesian(cameraPositionSpherical),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        renderer.updateAtoms(atoms);
        renderer.draw(false);

        impostorRenderer.setView(view);
        impostorRenderer.setLighting
        (
            spherical2cartesian(cameraPositionSpherical),
            {1.0f, 1.0f, 1.0f},
            0.1f
        );

        impostorRenderer.updateAtoms(impostorAtoms);
        impostorRenderer.draw(true);

        std::stringstream debugText;

        debugText << "Delta: " << fixedLengthNumber(delta,6) << " ms"
                  << " (FPS: " << fixedLengthNumber(1.0/(delta*1e-3),4)
                  << ")\n"
                  << "Triangles/Atom: \n" << "  Meshed: " << renderer.triangles() << "/" << atoms.size() << "\n"
                  << "  Impostors: " << impostorRenderer.triangles(true) << "/" << atoms.size() << "\n"
                  << vendor << " " << device;

        jGLInstance->text(
            debugText.str(),
            glm::vec2(64.0f, resY-32.0f),
            0.25f,
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

    if (MSAA > 0)
    {
        glDeleteRenderbuffers(1, &rbo);
    }
    jGLInstance->finish();

    return 0;
}