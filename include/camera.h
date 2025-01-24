#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

#include <atom.h>
#include <util.h>

/**
 * @brief A 3D projective camera centered on a focus moving on a sphere.
 * @remark The axis are OpenGL form e.g.
 * OpenGL Axes.
 *       +y |   -z
 *          | /
 *          |/
 *   -x ----/--- +x
 *         /|
 *     +z / | -y
 * @remark The spherical coordinates are length,
 * polar angle (resp. +y), and azimuthal angle (resp. +x);
 */
class Camera
{
public:

    /**
     * @brief Construct a new Camera focussing on some Atoms.
     *
     * @param atoms the atoms to fit into view.
     * @param resX the screen resolution width.
     * @param resY the screen resolution in height.
     */
    Camera(const std::vector<Atom> & atoms, uint16_t resX, uint16_t resY)
    : resX(resX), resY(resY)
    {
        reset(atoms);
    }

    /**
     * @brief Set the default view.
     * @remark reset() sets the camera to its default position. Far enough
     * to see all the atoms, focussing on {0,0,0} and up point +tve y.
     * @param atoms the Atoms to fit in view.
     */
    void reset(const std::vector<Atom> & atoms)
    {
        glm::vec3 ext = extent(atoms);
        positionSpherical = glm::vec3
        (
            2.0f*std::max(std::max(ext.x, ext.y), ext.z),
            M_PI*0.5f,
            M_PI
        );
        focus = {0.0, 0.0, 0.0};
        polarDirection = 1.0;
        up = 1.0;

        projection = glm::perspective
        (
            glm::radians(45.0f), float(resX)/float(resY),
            0.1f,
            1000.0f
        );

        view = glm::lookAt
        (
            spherical2cartesian(positionSpherical),
            focus,
            glm::vec3(0.0, up, 0.0)
        );
    }

    /**
     * @brief Increment the zoom.
     *
     * @param increment the zoom increase or decrease.
     */
    void zoom(float increment) { positionSpherical.x += increment; setView(); }

    /**
     * @brief Incline about the y OpenGL axis.
     *
     * @param increment the inclination increment.
     */
    void incline(float increment)
    {
        positionSpherical.y += increment*polarDirection;
        if (positionSpherical.y > M_PI || positionSpherical.y < 0.0)
        {
            positionSpherical.y -= 2.0*increment*polarDirection;
            polarDirection = -1.0*polarDirection;
            positionSpherical.z += M_PI;
            if ( positionSpherical.z < 0) { positionSpherical.z += 2.0*M_PI; }
            else if ( positionSpherical.z > 2.0*M_PI) { positionSpherical.z = std::fmod(positionSpherical.z, 2.0*M_PI); }
            up *= -1.0;
        }
        setView();
    }

    /**
     * @brief Rotate about the y OpenGL axis.
     *
     * @param increment the rotation increment.
     */
    void rotate(float increment)
    {
        positionSpherical.z -= increment;
        if ( positionSpherical.z < 0) { positionSpherical.z += 2.0*M_PI; }
        else if ( positionSpherical.z > 2.0*M_PI) { positionSpherical.z = std::fmod(positionSpherical.z, 2.0*M_PI); }
        setView();
    }

    /**
     * @brief Return the cartesian position vector.
     *
     * @return glm::vec3 the cartesian position of the camera.
     */
    glm::vec3 position() const { return spherical2cartesian(positionSpherical); }

    /**
     * @brief Get the Projection matrix.
     *
     * @return glm::mat4 the current projection.
     */
    glm::mat4 getProjection() const { return projection; }

    /**
     * @brief Get the View matrix.
     *
     * @return glm::mat4 the current view matrix.
     */
    glm::mat4 getView() const { return view; }

private:

    uint16_t resX;
    uint16_t resY;

    glm::vec3 positionSpherical;
    glm::vec3 focus;
    float polarDirection;
    float up;

    glm::mat4 projection;
    glm::mat4 view;

    void setView()
    {
        view = glm::lookAt
        (
            spherical2cartesian(positionSpherical),
            focus,
            glm::vec3(0.0, up, 0.0)
        );
    }

};

#endif /* CAMERA_H */
