#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "graph.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

#ifndef PATH_HPP
inline int min(int a, int b) { return (a < b) ? a : b; }
#endif

inline int mod(int x, int m) { return (x < 0 ? ((x % m) + m) % m : x % m); }

struct Cycle;
std::ostream &operator<<(std::ostream &os, Cycle &G);

struct Cycle : public graph::Graph {

  Cycle(int _n) : Graph(_n) {
    for (int i = n - 1; i >= 0; i--) {
      matrix[i][(i + 1) % n] = 1;
      matrix[(i + 1) % n][i] = 1;
    }
  }

  Cycle &operator++() {
    for (int i = n - 1; i >= 0; i--) {
      if (i == 0) {
        end = true;
        break;
      }
      matrix[i][(i + 1) % n] = (matrix[i][(i + 1) % n] % 2) + 1;
      matrix[(i + 1) % n][i] = matrix[i][(i + 1) % n];
      if (matrix[i][(i + 1) % n] == 2) {
        break;
      }
    }
    return *this;
  }

  void save(std::string filename) {
    std::ofstream file(filename);
    file << "graph {" << '\n';
    for (int i = 0; i < n; i++) {
      file << (char)('a' + i) << "[label=" << color[i] << "]\n";
    }
    for (int i = 0; i < n - 1; i++) {
      if (matrix[i][(i + 1) % n] == 1) {
        file << (char)('a' + i) << " -- " << (char)('a' + i + 1) << ";\n";
      } else {
        file << (char)('a' + i) << " -- " << (char)('a' + i + 1)
             << " [style=dotted];\n";
      }
    }
    if (matrix[0][n - 1] == 1) {
      file << (char)('a' + 0) << " -- " << (char)('a' + n - 1) << ";\n";
    } else {
      file << (char)('a' + 0) << " -- " << (char)('a' + n - 1)
           << " [style=dotted];\n";
    }
    file << "}\n";
    file.close();
  }

  static Cycle An(int n) {
    Cycle G(n);
    for (int i = 0; i < n; i++) {
      G.matrix[i][(i + 1) % n] = i % 2;
      G.matrix[(i + 1) % n][i] = i % 2;
    }
    return G;
  }
};

std::ostream &operator<<(std::ostream &os, Cycle &G) {
  for (int i = 0; i < G.n; i++) {
    os << G.matrix[i][(i + 1) % G.n] << " ";
  }
  os << "\n";
  for (int i = 0; i < G.n; i++) {
    os << G.color[i] << " ";
  }
  os << "\n";
  return os;
}
#endif
