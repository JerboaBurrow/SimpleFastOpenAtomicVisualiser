#ifndef TRIANGLE__H
#define TRIANGLE__H

#include <cstdint>
#include <vector>
#include <array>
#include <cmath>

#include <glm/glm.hpp>

template <class T> using vec3 = glm::vec<3,T,glm::packed_highp>;

template <class T>
class Triangle
{
public:

  Triangle()
  {
    x = vec3<T> (0);
    y = vec3<T> (0);
    z = vec3<T> (0);
  };

  Triangle(vec3<T>  v1,vec3<T>  v2,vec3<T>  v3)
  : x(v1),y(v2),z(v3)
  {};

  std::array<vec3<T>, 3> getVertices() const
  {
    std::array<vec3<T>, 3> v = {};
    for (uint32_t j = 0; j < 3; j++)
    {
      v[0][j] = x[j];
      v[1][j] = y[j];
      v[2][j] = z[j];
    }
    return v;
  }

  void setVertices(std::array<vec3<T>, 3> v)
  {
    for (uint32_t j = 0; j < 3; j++)
    {
      x[j] = v[0][j];
      y[j] = v[1][j];
      z[j] = v[2][j];
    }
  }

  vec3<T> normal() const
  {
    vec3<T> u = y-x;
    vec3<T> v = z-x;
    return glm::normalize(glm::cross(u, v));
  }

  vec3<T>  x;
  vec3<T>  y;
  vec3<T>  z;
};

#endif /* TRIANGLE__H */
