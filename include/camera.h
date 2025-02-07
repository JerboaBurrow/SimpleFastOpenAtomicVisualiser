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
     * @brief Construct a new Camera at a given position.
     *
     * @param positionSpherical the camera's position in spherical coordinates.
     * @param resX the screen resolution width.
     * @param resY the screen resolution in height.
     */
    Camera(glm::vec3 positionSpherical, uint16_t resX, uint16_t resY)
    : resX(resX), resY(resY), positionSpherical(positionSpherical)
    {
        reset();
    }

    /**
     * @brief Set the default view.
     */
    void reset()
    {
        focus = {0.0, 0.0, 0.0};
        up = 1.0;

        projection = glm::perspective
        (
            glm::radians(45.0f), float(resX)/float(resY),
            0.1f,
            1000.0f
        );
        invProjection = glm::inverse(projection);

        view = glm::lookAt
        (
            spherical2cartesian(positionSpherical),
            focus,
            glm::vec3(0.0, up, 0.0)
        );
        invView = glm::inverse(view);

        pv = projection*view;
        invPv = glm::inverse(pv);
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
        reset();
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
        positionSpherical.y += increment*up;
        if (positionSpherical.y > M_PI || positionSpherical.y < 0.0)
        {
            positionSpherical.y -= 2.0*increment*up;
            up = -1.0*up;
            positionSpherical.z += M_PI;
            if ( positionSpherical.z < 0) { positionSpherical.z += 2.0*M_PI; }
            else if ( positionSpherical.z > 2.0*M_PI) { positionSpherical.z = std::fmod(positionSpherical.z, 2.0*M_PI); }
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
     * @brief Set the camera's position.
     *
     * @remark Updates the view.
     * @param positionSpherical the position in spherical coordinates.
     */
    void setPosition(glm::vec3 positionSpherical)
    {
        this->positionSpherical = positionSpherical;
        setView();
    }

    /**
     * @brief Return the cartesian position vector.
     *
     * @param spherical if true return the position in spherical coordinates.
     *  Otherwise cartesian.
     * @return glm::vec3 the cartesian position of the camera.
     */
    glm::vec3 position(bool spherical = false) const
    {
        return spherical ? positionSpherical : spherical2cartesian(positionSpherical);
    }

    /**
     * @brief Set the up direction.
     *
     * @remark Updates the view.
     * @param up the up direction.
     */
    void setUp(float up) { this->up = up < 0 ? -1.0 : 1.0; setView(); }

    /**
     * @brief Get the up direction.
     *
     * @return float
     */
    float getUp() const { return up; }

    /**
     * @brief Get the Projection matrix.
     *
     * @return glm::mat4 the current projection.
     */
    glm::mat4 getProjection() const { return projection; }

    /**
     * @brief Get the inverse Projection matrix.
     *
     * @return glm::mat4 the current inverse projection.
     */
    glm::mat4 getInverseProjection() const { return invProjection; }

    /**
     * @brief Get the View matrix.
     *
     * @return glm::mat4 the current view matrix.
     */
    glm::mat4 getView() const { return view; }

    /**
     * @brief Get the inverse View matrix.
     *
     * @return glm::mat4 the current inverse view matrix.
     */
    glm::mat4 getInverseView() const { return invView; }

    /**
     * @brief Get the Projection*View matrix.
     *
     * @return glm::mat4 the current Projection*View matrix.
     */
    glm::mat4 getPV() const { return pv; }

    /**
     * @brief Get the inverse Projection*View matrix.
     *
     * @return glm::mat4 the current inverse Projection*View matrix.
     */
    glm::mat4 getInversePV() const { return invPv; }


    uint16_t getResX() const { return resX; }
    uint16_t getResY() const { return resY; }

private:

    uint16_t resX;
    uint16_t resY;

    glm::vec3 positionSpherical;
    glm::vec3 focus;
    float up;

    glm::mat4 projection;
    glm::mat4 invProjection;
    glm::mat4 view;
    glm::mat4 invView;
    glm::mat4 pv;
    glm::mat4 invPv;

    void setView()
    {
        view = glm::lookAt
        (
            spherical2cartesian(positionSpherical),
            focus,
            glm::vec3(0.0, up, 0.0)
        );
        invView = glm::inverse(view);
        pv = projection*view;
        invPv = glm::inverse(pv);
    }

};

#endif /* CAMERA_H */
