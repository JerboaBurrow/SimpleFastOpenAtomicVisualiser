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

#include <glm/glm.hpp>

const uint32_t NULL_ID = std::numeric_limits<uint32_t>::max(); 

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

template <class T>
class Trixel: public Triangle<T>
{
private:

  std::string ID;
  uint32_t parent;
  std::vector<uint32_t> children;

public:

  Trixel()
  : Triangle<T>(), ID(""), parent(NULL_ID), children(std::vector<uint32_t>(4,NULL_ID))
  {};

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

  std::string getID() const
  {
    return ID;
  }

  void setParent(uint32_t & p)
  {
    parent = p;
  }

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

  const std::vector<uint32_t> & getChildren() const
  {
    return children;
  }

};

template <class T>
std::array<Trixel<T>, 4> subdivideTrixel(Trixel<T> & trix)
{
  std::array<vec3<T>, 3> v = trix.getVertices();
  vec3<T>  w0 = vec3<T> (0);
  vec3<T>  w1 = vec3<T> (0);
  vec3<T>  w2 = vec3<T> (0);

  w0[0] = v[2][0]+v[1][0];
  w0[1] = v[2][1]+v[1][1];
  w0[2] = v[2][2]+v[1][2];

  w1[0] = v[2][0]+v[0][0];
  w1[1] = v[2][1]+v[0][1];
  w1[2] = v[2][2]+v[0][2];

  w2[0] = v[1][0]+v[0][0];
  w2[1] = v[1][1]+v[0][1];
  w2[2] = v[1][2]+v[0][2];

  T n0 = sqrt(w0[0]*w0[0]+w0[1]*w0[1]+w0[2]*w0[2]);
  T n1 = sqrt(w1[0]*w1[0]+w1[1]*w1[1]+w1[2]*w1[2]);
  T n2 = sqrt(w2[0]*w2[0]+w2[1]*w2[1]+w2[2]*w2[2]);

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

template <class T>
class HTM
{
public:

  HTM()
  : depth(0), Mesh(std::vector<Trixel<T>>(0))
  {
    // depth 0 hard coded
    vec3<T>  v0 {0.0,0.0,1.0};
    vec3<T>  v1 {1.0,0.0,0.0};
    vec3<T>  v2 {0.0,1.0,0.0};
    vec3<T>  v3 {-1.0,0.0,0.0};
    vec3<T>  v4 {0.0,-1.0,0.0};
    vec3<T>  v5 {0.0,0.0,-1.0};

    Trixel<T> T1("S0",v1,v5,v2,3);
    Trixel<T> T2("N0",v1,v0,v4,3);
    Trixel<T> T3("S1",v2,v5,v3,3);
    Trixel<T> T4("N1",v4,v0,v3,3);
    Trixel<T> T5("S2",v3,v5,v4,3);
    Trixel<T> T6("N2",v3,v0,v2,3);
    Trixel<T> T7("S3",v4,v5,v1,3);
    Trixel<T> T8("N3",v2,v0,v1,3);
    std::vector<Trixel<T>> u {T1,T2,T3,T4,T5,T6,T7,T8};
    Mesh = u;
    depth = depth;
  };

  size_t size() const
  {
    return depth;
  }

  void build(size_t depth = 0)
  {
    if (depth == 0)
    {
      return;
    }
    for (uint32_t i = 0; i < depth; i++)
    {
      uint32_t start = Mesh.size()-8*pow(4,i);
      uint32_t end = Mesh.size();
      for (uint32_t j = start; j < end; j++)
      {
        std::array<Trixel<T>, 4> newTrixels = subdivideTrixel<T> (Mesh[j]);
        std::vector<uint32_t> children(4,0);

        for (uint32_t k = 0; k < newTrixels.size(); k++)
        {
          newTrixels[k].setParent(j);
          Mesh.push_back(newTrixels[k]);
          children[k] = Mesh.size()-1;
        }
        Mesh[j].setChildren(children);
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
      l.push_back(Mesh[i]);
    }
    return l;
  }

  std::vector<T> vertices() const
  {
    std::vector<T> v;
    std::vector<uint32_t> leaves = leafIndices();
    for (uint32_t i = 0; i < leaves.size(); i++)
    {
      Trixel<T> trix = Mesh[leaves[i]];
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
      Trixel<T> trix = Mesh[leaves[i]];
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
        Trixel<T> trix = Mesh[leaves[i]];
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

  size_t depth;
  std::vector<Trixel<T>> Mesh;

  std::vector<uint32_t> leafIndices() const
  {                                                    
    std::vector<uint32_t> leaves;
    std::vector<uint32_t> stack {0,1,2,3,4,5,6,7};

    while (stack.size() > 0)
    {
      uint32_t ptr = stack[stack.size()-1];
      stack.pop_back();
      Trixel<T> trix = Mesh[ptr];
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
#endif
