#ifndef HIERARCHICALTRIANGULARMESH_H
#define HIERARCHICALTRIANGULARMESH_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include <cmath>
#include <memory>
#include <limits>
#include <numeric>

#include <glm/glm.hpp>

#include <trixel.h>
#include <meshes.h>

/**
 * @brief Base mesh types for refinement.
 *
 */
enum class BASE_MESH : uint8_t {ICOSAHEDRON, OCTAHEDRON, DODECAHEDRON, CUBE, TETRAHEDRON, TRIAUGMENTED_TRIANGULAR_PRISM, ANY};

/**
 * @brief A refineable mesh of triangles.
 *
 * @tparam T numeric type.
 */
template <class T>
class HierarchicalTriangularMesh
{
public:

  /**
   * @brief Construct a new Hierarchical Triangular Mesh from a library base.
   *
   * @param mesh either a specific base or any. @see meshes.h.
   */
  HierarchicalTriangularMesh(BASE_MESH mesh)
  : depth(0)
  {
    // depth 0 hard coded
    if (mesh == BASE_MESH::OCTAHEDRON)
    {
      this->mesh = OCTAHEDRON<T>;
    }
    else if (mesh == BASE_MESH::ICOSAHEDRON)
    {
      this->mesh = ICOSAHEDRON<T>;
    }
    else if (mesh == BASE_MESH::CUBE)
    {
      this->mesh = CUBE<T>;
    }
    else if (mesh == BASE_MESH::DODECAHEDRON)
    {
      this->mesh = DODECAHEDRON<T>;
    }
    else if (mesh == BASE_MESH::TETRAHEDRON)
    {
      this->mesh = TETRAHEDRON<T>;
    }
    else if (mesh == BASE_MESH::TRIAUGMENTED_TRIANGULAR_PRISM)
    {
      this->mesh = TRIAUGMENTED_TRIANGULAR_PRISM<T>;
    }
    centreMesh<T>(this->mesh);
    rootMeshSize = this->mesh.size();
  };

  /**
   * @brief Construct a new Hierarchical Triangular Mesh from a user base mesh.
   *
   * @param baseMesh a user supplied triangulation.
   */
  HierarchicalTriangularMesh(const std::vector<Trixel<T>> & baseMesh)
  : depth(0), mesh(baseMesh)
  {
    rootMeshSize = this->mesh.size();
    centreMesh<T>(this->mesh);
  };

  /**
   * @brief Get the number of refinements.
   *
   * @return uint32_t refinements calculated.
   */
  uint32_t size() const { return depth; }

  /**
   * @brief Get the number of triangles at the largest refinement.
   *
   * @return uint32_t the number of triangles.
   */
  uint32_t triangles() const { return rootMeshSize * std::pow(4, depth); }

  /**
   * @brief Refine the mesh to a give depth.
   *
   * @param depth depth of refinement.
   */
  void build(uint32_t depth = 0)
  {
    this->depth = depth;
    if (depth == 0)
    {
      return;
    }
    for (uint32_t i = 0; i < depth; i++)
    {
      uint32_t start = mesh.size()-rootMeshSize*pow(4,i);
      uint32_t end = mesh.size();
      for (uint32_t j = start; j < end; j++)
      {
        std::array<Trixel<T>, 4> newTrixels = subdivideTrixel<T> (mesh[j]);
        std::vector<uint32_t> children(4,0);

        for (uint32_t k = 0; k < newTrixels.size(); k++)
        {
          newTrixels[k].setParent(j);
          mesh.push_back(newTrixels[k]);
          children[k] = mesh.size()-1;
        }
        mesh[j].setChildren(children);
      }
    }
    return;
  }

  /**
   * @brief The triangulation at the current refinement.
   *
   * @return std::vector<Trixel<T>> the triangulation.
   */
  std::vector<Trixel<T>> leaves() const
  {
    std::vector<Trixel<T>> l;
    std::vector<uint32_t> indices = leafIndices();
    for (uint32_t i = 0; i < indices.size(); i++)
    {
      l.push_back(mesh[i]);
    }
    return l;
  }

  /**
   * @brief Get the vertices of the mesh.
   *
   * @return std::vector<T> vertices flattened for each triangle.
   */
  std::vector<T> vertices() const
  {
    std::vector<T> v;
    std::vector<uint32_t> leaves = leafIndices();
    for (uint32_t i = 0; i < leaves.size(); i++)
    {
      Trixel<T> trix = mesh[leaves[i]];
      auto u = trix.getVertices();
      for (uint32_t j = 0; j < u.size(); j++)
      {
        for (uint32_t k = 0; k < 3; k++)
        {
          v.push_back(u[j][k]);
        }
      }
    }
    return v;
  }

  /**
   * @brief Get the normal vectors for each triangle.
   *
   * @return std::vector<T> the normal vectors, flattened.
   */
  std::vector<T> vertexNormals() const
  {
    std::vector<T> n;
    std::vector<uint32_t> leaves = leafIndices();
    for (uint32_t i = 0; i < leaves.size(); i++)
    {
      Trixel<T> trix = mesh[leaves[i]];
      auto u = trix.normal();
      for (uint32_t j = 0; j < 3; j++)
      {
        for (uint32_t k = 0; k < 3; k++)
        {
          n.push_back(u[k]);
        }
      }
    }
    return n;
  }

private:

  uint32_t depth;
  uint32_t rootMeshSize;
  std::vector<Trixel<T>> mesh;

  std::vector<uint32_t> leafIndices() const
  {
    std::vector<uint32_t> leaves;
    std::vector<uint32_t> stack;
    stack.resize(rootMeshSize);
    std::iota(stack.begin(), stack.end(), 0);

    while (stack.size() > 0)
    {
      uint32_t ptr = stack[stack.size()-1];
      stack.pop_back();
      Trixel<T> trix = mesh[ptr];
      if ((trix.getChildren()).size() == 0)
      {
        leaves.push_back(ptr);
        continue;
      }

      bool cont = false;
      for (uint32_t i = 0; i < (trix.getChildren()).size(); i++)
      {
        if ((trix.getChildren())[i] == NULL_ID)
        {
          leaves.push_back(ptr);
          cont = true;
          break;
        }
      }
      if (cont)
      {
        continue;
      }
      else
      {
        for (uint32_t i = 0; i < (trix.getChildren()).size(); i++)
        {
          stack.push_back((trix.getChildren())[i]);
        }
      }
    }
    return leaves;
  }
};

/**
 * @brief HierarchicalTriangularMesh ordering.
 *
 * @remark Ordered by number of triangles.
 * @tparam T numeric type.
 * @param a mesh a.
 * @param b mesh b.
 * @return true if a has less triangles than b.
 * @return false if a has more than or the same triangles as b.
 */
template <class T>
bool operator <(const HierarchicalTriangularMesh<T> & a, const HierarchicalTriangularMesh<T> & b) { return a.triangles() < b.triangles(); }

#endif /* HIERARCHICALTRIANGULARMESH_H */
