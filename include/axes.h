#ifndef AXES_H
#define AXES_H

#include <memory.h>

#include <bondRenderer.h>

/**
 * @brief Render the coordinate axes.
 * @remark Renders a fixed view of the coordinate axes's orientation.
 */
class Axes
{

public:

    Axes(const Camera & camera)
    : camera({3.5, M_PI*0.5f, M_PI}, camera.getResX(), camera.getResY())
    {
        renderer = std::make_unique<BondRenderer>(axes, axesPoints, axes.size());
        renderer->setBondScale(0.33f);
        renderer->setClipCorrection(3.0f);
    }

    /**
     * @brief Update renderer from a Camera.
     *
     * @param camera the camera to update from.
     */
    void updateCamera(const Camera & camera)
    {
        glm::vec3 pos = camera.position(true);
        pos.x = this->camera.position(true).x;
        this->camera.setPosition(pos);
        this->camera.setUp(camera.getUp());
        renderer->updateCamera(this->camera);
    }

    /**
     * @brief Draw the coordinate axes.
     *
     * @param viewport the viewport to draw the axes in.
     */
    void draw(glm::ivec4 viewport = {10, 10, 85, 85})
    {
        GLint currentViewPort[4];
        glGetIntegerv(GL_VIEWPORT, currentViewPort);
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
        renderer->draw();
        glViewport
        (
            currentViewPort[0],
            currentViewPort[1],
            currentViewPort[2],
            currentViewPort[3]
        );
    }

private:

    Camera camera;

    std::vector<Atom> axesPoints =
    {
        {Element::Unknown, {0,0,0}, 1.0f, {1.0f,1.0,1.0,1.0}},
        {Element::Unknown, {1,0,0}, 1.0f, {1.0f,0.0,0.0,1.0}},
        {Element::Unknown, {0,1,0}, 1.0f, {0.0f,1.0,0.0,1.0}},
        {Element::Unknown, {0,0,1}, 1.0f, {0.0f,0.0,1.0,1.0}}
    };

    const std::vector<Bond> axes {{0, 1}, {0, 2}, {0, 3}};

    std::unique_ptr<BondRenderer> renderer;
};

#endif /* AXES_H */
