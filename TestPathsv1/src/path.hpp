#ifndef PATH_HPP
#define PATH_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

#ifndef CYCLE_HPP
inline int min(int a, int b) { return (a < b) ? a : b; }
#endif
struct Path;
std::ostream &operator<<(std::ostream &os, Path &G);

struct Path {
  int n;
  std::vector<int> edges;
  std::vector<int> color;
  bool end = false;

  Path(int _n) : n(_n) {
    edges.resize(n - 1);
    color.resize(n);
    for (int i = 0; i < n - 1; i++) {
      edges[i] = 0;
      color[i] = 0;
    }
    color[n - 1] = 0;
  }

  int nb_neg() {
    int count = 0;
    for (int i = 0; i < n - 1; i++) {
      if (!edges[i])
        count++;
    }
    return count;
  }
  Path &operator++() {
    for (int i = n - 2; i >= 0; i--) {
      if (i == 0 && edges[0] == 1) {
        end = true;
        break;
      }
      edges[i] = (edges[i] + 1) % 2;
      if (edges[i] == 1) {
        break;
      }
    }
    return *this;
  }

  bool has_p_colors(int K, int p) {
    std::vector<bool> C = std::vector<bool>(K, false);
    for (int u = 0; u < n; u++) {
      C[color[u]] = true;
    }
    int nb = 0;
    for (int k = 0; k < K; k++) {
      if (C[k]) {
        nb++;
      }
    }
    return nb == p;
  }

  bool is_complete_coloration(int K) {
    if (!colorable(K, n) || !has_p_colors(K, K)) {
      return false;
    }
    for (int k = 0; k < K; k++) {
      for (int j = k + 1; j < K; j++) {
        bool state = false;
        for (int l = 0; l < n - 1 && !state; l++) {
          if (color[l] == k && color[l + 1] == j) {
            state = true;
          }
        }
        for (int l = 1; l < n && !state; l++) {
          if (color[l] == k && color[l - 1] == j) {
            state = true;
          }
        }
        for (int u = 0; u < n && !state; u++) {
          if (color[u] == k) {
            for (int v = 0; v < n && !state; v++) {
              if (color[v] == j) {
                if (u > 0 && v > 0 && color[u - 1] == color[v - 1] &&
                    edges[u - 1] != edges[v - 1]) {
                  state = true;
                }
                if (u < n - 1 && v > 0 && color[u + 1] == color[v - 1] &&
                    edges[u] != edges[v - 1]) {
                  state = true;
                }
                if (v < n - 1 && u < n - 1 && color[u + 1] == color[v + 1] &&
                    edges[u] != edges[v]) {
                  state = true;
                }
                if (0 < u && v < n - 1 && color[u - 1] == color[v + 1] &&
                    edges[u - 1] != edges[v]) {
                  state = true;
                }
              }
            }
          }
        }
        if (!state) {
          return false;
        }
      }
    }
    return true;
  }

  bool colorable(int K, int i) {
    // is the graph on the first i vertices colorable with current colors
    for (int l = 0; l < i - 1; l++) {
      if (color[l] == color[l + 1]) {
        return false;
      }
    }
    for (int u = 0; u < i; u++) {
      for (int v = u + 2; v < i; v++) {
        if (color[u] == color[v] && u != v) {
          if (u > 0 && color[u - 1] == color[v - 1] &&
              edges[u - 1] != edges[v - 1]) {
            return false;
          }
          if (u < i - 1 && color[u + 1] == color[v - 1] &&
              edges[u] != edges[v - 1]) {
            return false;
          }
          if (v < i - 1 && color[u + 1] == color[v + 1] &&
              edges[u] != edges[v]) {
            return false;
          }
          if (0 < u && v < i - 1 && color[u - 1] == color[v + 1] &&
              edges[u - 1] != edges[v]) {
            return false;
          }
        }
      }
    }
    return true;
  }

  int coloring(int K_min) {
    int Kmax = 0;
    Path Gmax = *this;
    for (int K = K_min; K <= 4; K++) {
      // std::cout << "loop\n";
      // std::cout << K << std::endl;
      // std::cout << Gmax;
      if (Kmax == K) {
        continue;
      }
      int u = 0;
      color[u] = 0;
      bool iscolorable = false;
      while (1) {
        // std::cout << "insidewhile " << K << " " <<  u << "\n" << *this <<
        // '\n';
        if (u == 0 && color[u] >= min(u + 1, K)) {
          // std::cout << "breakcond\n";
          break;
        }
        if (u == n && is_complete_coloration(K)) {
          // std::cout << "complete\n";
          iscolorable = true;
          Kmax = K;
          Gmax = *this;
          break;
        }
        if (u == n && colorable(K, n)) {
          // std::cout << "u=n\n";
          iscolorable = true;
          u--;
          color[u]++;
          continue;
        }
        if (color[u] >= min(u + 1, K)) {
          // std::cout << "color >= K\n";
          color[u] = 0;
          u--;
          color[u]++;
          continue;
        }
        if (colorable(K, u + 1)) {
          // std::cout << "colorable\n";
          u++;
          if (u < n) {
            color[u] = 0;
          }
        } else {
          // std::cout << "else\n";
          color[u]++;
        }
      }
      if (Kmax != K && iscolorable) {
        *this = Gmax;
        return Kmax;
      }
    }
    *this = Gmax;
    return Kmax;
  }

  void save(std::string filename) {
    std::ofstream file(filename);
    file << "graph {" << '\n';
    for (int i = 0; i < n; i++) {
      file << (char)('a' + i) << "[label=" << color[i] << "]\n";
    }
    for (int i = 0; i < n - 1; i++) {
      if (edges[i] == 1) {
        file << (char)('a' + i) << " -- " << (char)('a' + i + 1) << ";\n";
      } else {
        file << (char)('a' + i) << " -- " << (char)('a' + i + 1)
             << " [style=dotted];\n";
      }
    }
    file << "}\n";
    file.close();
  }

  void latex() {
    if (n != 8) {
      return;
    }
    std::cout
        << "\\subfloat{\n\t\\begin{tikzpicture}\n\t\t\\tikzstyle{n} = [ draw, "
           "circle, minimum size = 1mm, text centered ];\n\t\t\\tikzstyle{g} "
           "= [ ForestGreen, line width = 1pt ];\n\t\t\\tikzstyle{r} = [ red, "
           "line width = 1pt ];\n\n ";

    std::cout << "\t\t\\node[n] (0) at(0, 0) {" << color[0] << "};\n";
    std::cout << "\t\t\\node[n] (1) at(2, 0) {" << color[1] << "};\n";
    std::cout << "\t\t\\node[n] (2) at(4, 0) {" << color[2] << "};\n";
    std::cout << "\t\t\\node[n] (3) at(6, 0) {" << color[3] << "};\n";
    std::cout << "\t\t\\node[n] (4) at(8, 0) {" << color[4] << "};\n";
    std::cout << "\t\t\\node[n] (5) at(10, 0) {" << color[5] << "};\n";
    std::cout << "\t\t\\node[n] (6) at(12, 0) {" << color[6] << "};\n";
    std::cout << "\t\t\\node[n] (7) at(14, 0) {" << color[7] << "};\n";

    for (int i = 0; i < n - 1; i++) {
      std::cout << "\t\t\\draw(" << i << ") edge[";
      if (edges[i]) {
        std::cout << "g";
      } else {
        std::cout << "r";
      }
      std::cout << "](" << i + 1 << ");\n";
    }
    std::cout << "\t\\end {tikzpicture}\n}\n\n";
  }
};

std::ostream &operator<<(std::ostream &os, Path &G) {
  for (int i = 0; i < G.n - 1; i++) {
    os << G.edges[i] << " ";
  }
  os << "\n";
  for (int i = 0; i < G.n; i++) {
    os << G.color[i] << " ";
  }
  os << "\n";
  return os;
}
#endif
