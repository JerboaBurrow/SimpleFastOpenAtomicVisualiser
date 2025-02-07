#ifndef GLUTILS_H
#define GLUTILS_H

#include <glm/glm.hpp>

#include <jGL/OpenGL/gl.h>

/**
 * @brief Substitute a GL_ARRAY_BUFFER fully.
 *
 * @param buffer the GL_ARRAY_BUFFER id
 * @param data the new data.
 * @param size the size.
 */
void subFullBuffer(GLuint & buffer, float * data, GLuint size)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferSubData
        (
            GL_ARRAY_BUFFER,
            0,
            sizeof(float)*size,
            data
        );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Enable a GL_ARRAY_BUFFER.
 *
 * @param buffer the GL_ARRAY_BUFFER id.
 * @param attribute the vertex attributed.
 * @param size the dimension.
 * @param divisor the intancing divisor.
 */
void enableBuffer
(
    GLuint & buffer,
    GLuint attribute,
    GLuint size,
    GLuint divisor
)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(attribute);
        glVertexAttribPointer
        (
            attribute,
            size,
            GL_FLOAT,
            false,
            size*sizeof(float),
            0
        );
        glVertexAttribDivisor(attribute, divisor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief Create a GL_ARRAY_BUFFER from data.
 *
 * @param buffer the GL_ARRAY_BUFFER id.
 * @param data the data to upload.
 * @param dataSize the amount of data.
 * @param drawType the draw type.
 * @param attribute the vertex attribute.
 * @param size dimension.
 * @param divisor the instance divisor.
 */
void createBuffer
(
    GLuint & buffer,
    const float * data,
    GLuint dataSize,
    int drawType,
    GLuint attribute,
    GLuint size,
    GLuint divisor
)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glEnableVertexAttribArray(attribute);
        glBufferData
        (
            GL_ARRAY_BUFFER,
            sizeof(float)*dataSize,
            data,
            drawType
        );
        glVertexAttribPointer
        (
            attribute,
            size,
            GL_FLOAT,
            false,
            size*sizeof(float),
            0
        );
        glVertexAttribDivisor(attribute, divisor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif /* GLUTILS_H */
