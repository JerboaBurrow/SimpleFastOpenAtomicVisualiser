#ifndef HTM_H
#define HTM_H

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

enum class BASE_MESH : uint8_t {ICOSAHEDRON, OCTAHEDRON, DODECAHEDRON, CUBE, TETRAHEDRON, TRIAUGMENTED_TRIANGULAR_PRISM, ANY};

template <class T>
class HTM
{
public:

  HTM(BASE_MESH mesh)
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

  HTM(const std::vector<Trixel<T>> & baseMesh)
  : depth(0), mesh(baseMesh)
  {
    rootMeshSize = this->mesh.size();
    centreMesh<T>(this->mesh);
  };

  uint32_t size() const { return depth; }

  uint32_t triangles() const { return rootMeshSize * std::pow(4, depth); }

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

  void writeLeaves(const std::string & filename) const
  {
    std::ofstream output;
    output.open(filename);
    if (output.is_open())
    {
      std::vector<uint32_t> leaves = leafIndices();
      for (uint32_t i = 0; i < leaves.size(); i++)
      {
        Trixel<T> trix = mesh[leaves[i]];
        auto v = trix.getVertices();
        for (uint32_t j = 0; j < v.size(); j++)
        {
          for (uint32_t k = 0; k < 3; k++)
          {
            output << v[j][k] << ",";
          }
        }
        output << trix.getID() << std::endl;
      }
      output.close();
    }
    else
    {
      std::cout << "Error in reading output file: " << filename << std::endl;
      exit (EXIT_FAILURE);
    }
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

template <class T>
bool operator <(const HTM<T> & a, const HTM<T> & b) { return a.triangles() < b.triangles(); }

#endif /* HTM_H */
