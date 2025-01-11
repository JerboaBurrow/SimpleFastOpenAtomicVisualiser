#ifndef TRIXEL_H
#define TRIXEL_H

#include <triangle.h>

/**
 * @brief The null Trixel identity.
 *
 */
const uint32_t NULL_ID = std::numeric_limits<uint32_t>::max();

/**
 * @brief A triangular pixel of a hierarchical triangular mesh.
 * @remark @see HTM.
 * @tparam T numeric type.
 */
template <class T>
class Trixel: public Triangle<T>
{
public:

  /**
   * @brief Construct a new empty Trixel.
   *
   */
  Trixel()
  : Triangle<T>(), ID(""), parent(NULL_ID), children(std::vector<uint32_t>(4,NULL_ID))
  {};

  /**
   * @brief Construct a new Trixel
   *
   * @param id the identity.
   * @param x first vertex.
   * @param y second vertex.
   * @param z third vertex.
   * @param parent the parent Trixel.
   * @param children child Trixels.
   */
  Trixel
  (
    std::string id,
    vec3<T>  x,
    vec3<T>  y,
    vec3<T>  z,
    uint32_t parent = NULL_ID,
    std::vector<uint32_t> children = std::vector<uint32_t>(4,NULL_ID)
  )
  : Triangle<T>(x, y, z), ID(id), parent(parent), children(children)
  {};

  /**
   * @brief Return the Trixel's id
   *
   * @return std::string the string id.
   */
  std::string getID() const
  {
    return ID;
  }

  /**
   * @brief Set the Trixel's parent Trixel.
   *
   * @param p the parent's id.
   */
  void setParent(uint32_t & p)
  {
    parent = p;
  }

  /**
   * @brief Set the Trixel's children.
   *
   * @param c childrens' ids.
   */
  void setChildren(std::vector<uint32_t> & c)
  {
    if (c.size() == children.size())
    {
      for (uint32_t i = 0; i < c.size(); i++)
      {
        children[i] = c[i];
      }
    }
  }

  /**
   * @brief Get the Trixel's children.
   *
   * @return const std::vector<uint32_t>& childrens' ids.
   */
  const std::vector<uint32_t> & getChildren() const
  {
    return children;
  }

private:

  std::string ID;
  uint32_t parent;
  std::vector<uint32_t> children;

};

/**
 * @brief Subdivide a Trixel.
 *
 * @remark Subdivides into 4 Trixels (like a Triforce).
 * @tparam T numeric type.
 * @param trix the Trixel to subdivide.
 * @return std::array<Trixel<T>, 4> The resulting subdivision.
 */
template <class T>
std::array<Trixel<T>, 4> subdivideTrixel(Trixel<T> & trix)
{
  std::array<vec3<T>, 3> v = trix.getVertices();
  vec3<T>  w0 = vec3<T> (0.0);
  vec3<T>  w1 = vec3<T> (0.0);
  vec3<T>  w2 = vec3<T> (0.0);

  w0[0] = v[2][0]+v[1][0];
  w0[1] = v[2][1]+v[1][1];
  w0[2] = v[2][2]+v[1][2];

  w1[0] = v[2][0]+v[0][0];
  w1[1] = v[2][1]+v[0][1];
  w1[2] = v[2][2]+v[0][2];

  w2[0] = v[1][0]+v[0][0];
  w2[1] = v[1][1]+v[0][1];
  w2[2] = v[1][2]+v[0][2];

  T n0 = std::sqrt(w0[0]*w0[0]+w0[1]*w0[1]+w0[2]*w0[2]);
  T n1 = std::sqrt(w1[0]*w1[0]+w1[1]*w1[1]+w1[2]*w1[2]);
  T n2 = std::sqrt(w2[0]*w2[0]+w2[1]*w2[1]+w2[2]*w2[2]);

  w0[0] = w0[0]/n0;
  w0[1] = w0[1]/n0;
  w0[2] = w0[2]/n0;

  w1[0] = w1[0]/n1;
  w1[1] = w1[1]/n1;
  w1[2] = w1[2]/n1;

  w2[0] = w2[0]/n2;
  w2[1] = w2[1]/n2;
  w2[2] = w2[2]/n2;

  vec3<T>  v0 {v[0][0],v[0][1],v[0][2]};
  vec3<T>  v1 {v[1][0],v[1][1],v[1][2]};
  vec3<T>  v2 {v[2][0],v[2][1],v[2][2]};

  std::string id = trix.getID();

  Trixel<T> T1(id+"0",v0,vec3<T> (w2),vec3<T> (w1));
  Trixel<T> T2(id+"1",v1,vec3<T> (w0),vec3<T> (w2));
  Trixel<T> T3(id+"2",v2,vec3<T> (w1),vec3<T> (w0));
  Trixel<T> T4(id+"3",vec3<T> (w0),vec3<T> (w1),vec3<T> (w2));
  std::array<Trixel<T>, 4> u {T1,T2,T3,T4};
  return u;
}

#endif /* TRIXEL_H */
