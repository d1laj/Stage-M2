#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <algorithm>
#include <array>
#include <iostream>

struct Literal {
  int value;
  Literal(int v) : value(v) {}

  Literal &operator-() {
    value *= -1;
    return *this;
  }
};

std::ostream &operator<<(std::ostream &os, Literal lit) {
  os << lit.value << " ";
  return os;
}

const std::string End = "0\n";

enum class VariableType {
  V = 0, // Does vertex u has color i
  Edge,
  EdgeSign, // Does uv exists and what is its sign
  Hom,
  HomSign,  // Does ij exits in homomorphic graph and what is its sign
  Color,    // Does color i is used
  X,        // Helper : does i and j clash through k or forms + path
  Complete, // Does the first i color are complete (forget the other colors)
  Comp,     // Does i and j are complete using the first k colors.
  Y,        // Does edge u has i-j with sign
  NbEdges,
  NumberOfTypes
};

struct ColorPair {
  int i = 0;
  int j = 1;
  int K;
  int N;

  ColorPair(int _K) : K(_K), N(K) {}
  ColorPair(int _K, int _N) : K(_K), N(_N) {}
  ColorPair(int _K, int _i, int _j)
      : i(std::min(_i, _j)), j(std::max(_i, _j)), K(_K), N(K) {}
  ColorPair(int _K, int _N, int _i, int _j)
      : i(std::min(_i, _j)), j(std::max(_i, _j)), K(_K), N(_N) {}

  ColorPair &operator++() {
    ++j;
    if (j >= K) {
      ++i;
      j = i + 1;
    }
    return *this;
  }

  ColorPair operator++(int junk) {
    ColorPair tmp = *this;
    ++(*this);
    return tmp;
  }

  bool is_end() { return !(i < K && j < K); }

  int index() { return i * N + j - (i + 1) * (i + 2) / 2; }
};

struct VertexPair {
  int u = 0;
  int v = 1;
  int N;
  VertexPair(int _N) : N(_N) {}
  VertexPair(int _N, int _u, int _v)
      : u(std::min(_u, _v)), v(std::max(_u, _v)), N(_N) {}

  VertexPair &operator++() {
    ++v;
    if (v >= N) {
      ++u;
      v = u + 1;
    }
    return *this;
  }

  VertexPair operator++(int junk) {
    VertexPair tmp = *this;
    ++(*this);
    return tmp;
  }

  VertexPair prev() {
    if (v == 0) {
      return VertexPair(N, u - 1, N - 1);
    } else {
      return VertexPair(N, u, v - 1);
    }
  }

  bool is_end() { return !(u < N && v < N); }

  int index() { return u * N + v - (u + 1) * (u + 2) / 2; }
};

unsigned to_int(VariableType t) { return static_cast<unsigned>(t); }

template <bool isTwoColo, bool edges = false> struct VariableOffsets {
  std::array<unsigned, static_cast<unsigned>(VariableType::NumberOfTypes)>
      varOffs;
  const int N;

  VariableOffsets(int _N) : N(_N) { fillOffsets(); }

  auto &operator[](const int i) { return varOffs[i]; }

  int fillOffsets() {
    int offset = 1;
    int index = 0;
    varOffs[index++] = offset;

    // V
    for (int u = 0; u < N; u++) {
      for (int i = 0; i < N; i++) {
        offset++;
      }
    }
    varOffs[index++] = offset;

    // Edge
    for (VertexPair uv(N); !uv.is_end(); uv++) {
      offset++;
    }
    varOffs[index++] = offset;

    // EdgeSign
    for (VertexPair uv(N); !uv.is_end(); uv++) {
      offset++;
    }
    varOffs[index++] = offset;

    // Hom
    for (ColorPair ij(N); !ij.is_end(); ij++) {
      offset++;
    }
    varOffs[index++] = offset;

    // HomSign
    for (ColorPair ij(N); !ij.is_end(); ij++) {
      offset++;
    }
    varOffs[index++] = offset;

    // Color
    for (int i = 0; i < N; i++) {
      offset++;
    }
    varOffs[index++] = offset;

    // X

    for (ColorPair ij(N); !ij.is_end(); ij++) {
      for (int k = 0; k < N; k++) {
        if (k != ij.i && k != ij.j) {
          offset++;
          if (!isTwoColo) {
            offset++;
          }
        }
      }
    }
    varOffs[index++] = offset;

    // Complete
    offset++;
    varOffs[index++] = offset;

    // Comp
    for (ColorPair ij(N); !ij.is_end(); ij++) {
      offset++;
    }
    varOffs[index++] = offset;

    // Y
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      for (ColorPair ij(N); !ij.is_end(); ij++) {
        offset += 2;
      }
    }

    if (edges) {
      varOffs[index++] = offset;

      // NbEdges
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        for (int i = 0; i <= N * (N - 1) / 2; i++) {
          offset++;
        }
      }
    }
    return 0;
  }

  Literal index(VariableType vt) {
    switch (vt) {
    case VariableType::Complete:
      return varOffs[to_int(vt)];
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, int i) {
    switch (vt) {
    case VariableType::Color:
      return varOffs[to_int(vt)] + i;
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, int u, int i) {
    switch (vt) {
    case VariableType::V:
      return varOffs[to_int(vt)] + u * N + i;
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, VertexPair vp) {
    switch (vt) {
    case VariableType::Edge:
    case VariableType::EdgeSign:
      return varOffs[to_int(vt)] + vp.index();
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, VertexPair vp, int i) {
    switch (vt) {
    case VariableType::NbEdges:
      return varOffs[to_int(vt)] + vp.index() * (N * (N - 1) / 2 + 1) + i;
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, ColorPair cp) {
    switch (vt) {
    case VariableType::Hom:
    case VariableType::HomSign:
    case VariableType::Comp:
      return varOffs[to_int(vt)] + cp.index();
      break;
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, ColorPair cp, int k) {
    switch (vt) {
    case VariableType::X:
      return varOffs[to_int(vt)] + cp.index() * (N - 2) +
             (k < cp.i ? k : (k < cp.j ? k - 1 : k - 2));
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, ColorPair cp, int k, bool sign) {
    switch (vt) {
    case VariableType::X:
      return varOffs[to_int(vt)] + cp.index() * (N - 2) * 2 +
             (k < cp.i ? k : (k < cp.j ? k - 1 : k - 2)) * 2 + (sign ? 0 : 1);
    default:
      return Literal(0);
    }
  }

  Literal index(VariableType vt, VertexPair vp, ColorPair cp, bool sign) {
    switch (vt) {
    case VariableType::Y:
      return varOffs[to_int(vt)] + vp.index() * N * (N - 1) + cp.index() * 2 +
             (sign ? 1 : 0);
      break;
    default:
      return Literal(0);
    }
  }

  void test_indices() {
    std::cout << "\nV\n";
    for (int u = 0; u < N; u++) {
      for (int i = 0; i < N; i++) {
        std::cout << "V " << u << " " << i << " is "
                  << index(VariableType::V, u, i) << "\n";
      }
    }

    std::cout << "\nEdge\n";
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      std::cout << "Edge " << vp.u << " " << vp.v << " is "
                << index(VariableType::Edge, vp) << "\n";
    }

    std::cout << "\nEdgeSign\n";
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      std::cout << "EdgeSign " << vp.u << " " << vp.v << " is "
                << index(VariableType::EdgeSign, vp) << "\n";
    }

    std::cout << "\nHom\n";
    for (ColorPair cp(N); !cp.is_end(); cp++) {
      std::cout << "Hom " << cp.i << " " << cp.j << " is "
                << index(VariableType::Hom, cp) << "\n";
    }

    std::cout << "\nEdgeSign\n";
    for (ColorPair cp(N); !cp.is_end(); cp++) {
      std::cout << "HomSign " << cp.i << " " << cp.j << " is "
                << index(VariableType::HomSign, cp) << "\n";
    }

    std::cout << "Color\n";
    for (int i = 0; i < N; i++) {
      std::cout << "Color " << i << " is " << index(VariableType::Color, i)
                << "\n";
    }

    std::cout << "\nX\n";
    for (ColorPair cp(N); !cp.is_end(); cp++) {
      for (int k = 0; k < N; k++) {
        if (k != cp.i && k != cp.j) {
          if (isTwoColo) {
            std::cout << "X " << cp.i << " " << cp.j << " " << k << " is "
                      << index(VariableType::X, cp, k) << "\n";
          } else {
            std::cout << "X++ " << cp.i << " " << cp.j << " " << k << " is "
                      << index(VariableType::X, cp, k, false) << "\n";
            std::cout << "X+- " << cp.i << " " << cp.j << " " << k << " is "
                      << index(VariableType::X, cp, k, true) << "\n";
          }
        }
      }
    }

    std::cout << "Complete\n";
    std::cout << "Complete "
              << " is " << index(VariableType::Complete) << "\n";

    std::cout << "\nComp\n";
    for (ColorPair cp(N); !cp.is_end(); cp++) {
      std::cout << "Comp " << cp.i << " " << cp.j << " is "
                << index(VariableType::Comp, cp) << "\n";
    }

    std::cout << "\nY\n";
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        std::cout << "Y- " << vp.u << " " << vp.v << " " << cp.i << " " << cp.j
                  << " is " << index(VariableType::Y, vp, cp, false) << "\n";
        std::cout << "Y+ " << vp.u << " " << vp.v << " " << cp.i << " " << cp.j
                  << " is " << index(VariableType::Y, vp, cp, true) << "\n";
      }
    }

    if (edges) {
      std::cout << "\nNbEdges\n";
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        for (int i = 0; i <= N * (N - 1) / 2; i++) {
          std::cout << "NbEdges " << vp.u << " " << vp.v << " " << i << " is "
                    << index(VariableType::NbEdges, vp, i) << "\n";
        }
      }
    }
  }
};
/*
enum class VariableType {
  CC = 0, // Does the graph has a complete coloring
  V,      // Does vertex u has color i
  Edge,
  EdgeSign, // Does uv exists and what is its sign
  Hom,
  HomSign,  // Does ij exits in homomorphic graph and what is its sign
  Color,    // Does color i is used
  X,        // Helper : does i and j clash through k
  Complete, // Does the first i color are complete (forget the other colors)
  Comp,     // Does i and j are complete using the first k colors.
  Y, // Does edge u has i-j with sign
  NumberOfTypes
};
*/

#endif
