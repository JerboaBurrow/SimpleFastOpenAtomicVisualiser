#ifndef ORTHOCAM_H
#define ORTHOCAM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace jGL
{
  /**
   * @brief An orthographic camera for 2D.
   * 
   * World coordinates are scaled to the size of the 
   * longest screen dimension in pixels. So [1, 1] is the
   * top right of the screen always.
   * 
   * Position is the bottom left of corner. Increasing y in world
   * space goes up the screen.
   * 
   */
  class OrthoCam 
  {

  public:

    /**
     * @brief Construct a new Ortho Cam object.
     * 
     * @param resx pixel resolution in x
     * @param resy pixel resolution in y
     */
    OrthoCam(int resx, int resy)
    : resolution(resx,resy), zoomLevel(1.0f), position(glm::vec2(0.0,0.0)) 
    {
      update();
    }

    /**
     * @brief Construct a new Ortho Cam object.
     * 
     * @param resx pixel resolution in x.
     * @param resy pixel resolution in y.
     * @param pos position in world coordinates.
     */
    OrthoCam(int resx, int resy, glm::vec2 pos)
    : resolution(resx,resy), zoomLevel(1.0f), position(pos) 
    {
      update();
    }

    /**
     * @brief Convert screen position to world position.
     * 
     * @param x screen x coordinate.
     * @param y screen y coordinate.
     * @return glm::vec4 world coordinates (wx, wy, _, 1.0)
     */
    glm::vec4 screenToWorld(float x, float y) const
    {
      glm::vec4 ndc(
        2.0*x/resolution.x-1.0,
        2.0*(resolution.y-y)/resolution.y-1.0,
        0.0,
        1.0
      );

      return invProjection*ndc;
    }

    /**
     * @brief Convert world position to screen coordinate.
     * 
     * @param x world x coordinate.
     * @param y world y coordinate.
     * @return glm::vec2 screen coordinate.
     */
    glm::vec2 worldToScreen(float x, float y) const
    {
      glm::vec4 pos = vp*glm::vec4(x, y, 0.0, 1.0);
      return glm::vec2( (pos.x+1.0)*resolution.x*0.5, -1.0*((pos.y+1.0)*resolution.y*0.5-resolution.y) );
    }

    const glm::mat4 & getVP() const {return vp;}
    
    const glm::mat4 getProjection() const {return projection;}

    glm::vec2 getResolution() const {return resolution;}

    glm::vec2 getPosition() const {return position;}

    float getZoomLevel() const {return zoomLevel;}
    void incrementZoom(float dz)
    {
      if (zoomLevel >= 1.0)
      {
        zoomLevel += dz;
        zoomLevel < 1.0 ? zoomLevel = 1.0 : 0;
      }
      else
      {
        zoomLevel += 1.0/dz;
      }
      update();

    }
    void setPosition(glm::vec2 newPosition){position=newPosition; update();}
    void setPosition(float x, float y){position=glm::vec2(x,y); update();}
    void move(float dx, float dy){position += glm::vec2(dx,dy); update();}

  private:

    void update()
    {
      // scale equally by screen width (all lengths relative to this)
      double maxRes = std::max(resolution.x,resolution.y);
      modelView = glm::scale(glm::mat4(1.0),glm::vec3(maxRes,maxRes,1.0)) *
      // move to position and look at x-y plane from z=1, with up = y axis
        glm::lookAt(
          glm::vec3(position.x,position.y,1.0),
          glm::vec3(position.x,position.y,0.0),
          glm::vec3(0.0,1.0,0.0)
        );

      glm::vec3 center(position.x+0.5,position.y+0.5, 1.0);
      modelView *= glm::translate(glm::mat4(1.0), center) *
            glm::scale(glm::mat4(1.0),glm::vec3(zoomLevel,zoomLevel,1.0))*
            glm::translate(glm::mat4(1.0), -center);

      // finally, project to the screen (ndc)
      projection = glm::ortho(
        0.0,
        double(resolution.x),
        0.0,
        double(resolution.y)
      );


      vp = projection*modelView;
      invProjection = glm::inverse(vp);
    }

    glm::vec2 resolution;
    glm::mat4 modelView;
    glm::mat4 projection;
    glm::mat4 invProjection;
    glm::mat4 vp;

    float zoomLevel;

    glm::vec2 position;
  };
}

#endif
