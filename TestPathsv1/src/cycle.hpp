#ifndef CYCLE_HPP
#define CYCLE_HPP

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

struct Cycle {
  int n;
  std::vector<int> edges;
  std::vector<int> color;
  bool end = false;

  Cycle(int _n) : n(_n) {
    edges.resize(n);
    color.resize(n);
    for (int i = 0; i < n; i++) {
      edges[i] = 0;
      color[i] = 0;
    }
  }

  Cycle &operator++() {
    for (int i = n - 1; i >= 0; i--) {
      if (i == 0) {
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
        for (int l = 0; l < n && !state; l++) {
          if (color[l] == k && color[(l + 1) % n] == j) {
            state = true;
          }
        }
        for (int l = 0; l < n && !state; l++) {
          if (color[(l + 1) % n] == k && color[l] == j) {
            state = true;
          }
        }
        for (int u = 0; u < n && !state; u++) {
          if (color[u] == k) {
            for (int v = 0; v < n && !state; v++) {
              if (color[v] == j) {
                if (color[mod(u - 1, n)] == color[mod(v - 1, n)] &&
                    edges[mod(u - 1, n)] != edges[mod(v - 1, n)]) {
                  state = true;
                }
                if (color[(u + 1) % n] == color[mod(v - 1, n)] &&
                    edges[u] != edges[mod(v - 1, n)]) {
                  state = true;
                }
                if (color[(u + 1) % n] == color[(v + 1) % n] &&
                    edges[u] != edges[v]) {
                  state = true;
                }
                if (color[mod(u - 1, n)] == color[(v + 1) % n] &&
                    edges[mod(u - 1, n)] != edges[v]) {
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
    if (i == n && color[0] == color[n - 1]) {
      return false;
    }
    for (int u = 0; u < i; u++) {
      for (int v = u + 2; v < i; v++) {
        if (color[u] == color[v] && u != v) {
          if ((u > 0 || i == n) &&
              color[mod(u - 1, n)] == color[mod(v - 1, n)] &&
              edges[mod(u - 1, n)] != edges[mod(v - 1, n)]) {
            return false;
          }
          if (color[(u + 1) % n] == color[mod(v - 1, n)] &&
              edges[u] != edges[mod(v - 1, n)]) {
            return false;
          }
          if ((v < i - 1 || i == n) &&
              color[(u + 1) % n] == color[(v + 1) % n] &&
              edges[u] != edges[v]) {
            return false;
          }
          if (((u > 0 && v < i - 1) || i == n) &&
              color[mod(u - 1, n)] == color[(v + 1) % n] &&
              edges[mod(u - 1, n)] != edges[v]) {
            return false;
          }
        }
      }
    }
    if (i == n) {
      return has_p_colors(K, K);
    }
    return true;
  }

  int coloring(int K_min) {
    int Kmax = 0;
    Cycle Gmax = *this;
    // std::cout << *this;
    bool once = false;
    for (int K = K_min; K <= 4; K++) {
      // std::cout << "loop\n";
      // std::cout << K;
      // std::cout << Gmax;
      if (Kmax == K) {
        continue;
      }
      int u = 0;
      color[u] = 0;
      bool iscolorable = false;
      while (1) {
        // std::cout << "insidewhile " << K << " " << u << "\n"; //<< *this <<
        // '\n';
        for (int i = 0; i < n; i++) {
          // std::cout << color[i] << " ";
        }
        // std::cout << '\n';
        if (u == 0 && color[u] >= min(u + 1, K)) {
          // std::cout << "breakcond\n";
          break;
        }
        if (u == n && is_complete_coloration(K)) {
          // std::cout << "complete\n";
          iscolorable = true;
          if (K == 4 && once) {
            once = false;
            std::cout << "colo:\n" << *this;
          }
          Kmax = K;
          Gmax = *this;
          break;
        }
        if (u == n && colorable(K, n)) {
          // std::cout << "u=n\n";
          iscolorable = true;
          if (K == 4 && once) {
            once = false;
            std::cout << "colo:\n" << *this;
          }
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
      if (Kmax != K && iscolorable && false) {
        *this = Gmax;
        // std::cout << "-->" << Kmax << '\n';
        return Kmax;
      } else if (iscolorable) {
        // std::cout << K;
      }
      // std::cout << " " << iscolorable << '\n';
    }
    // std::cout << "->" << Kmax << '\n';

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
    if (edges[n - 1] == 1) {
      file << (char)('a' + 0) << " -- " << (char)('a' + n - 1) << ";\n";
    } else {
      file << (char)('a' + 0) << " -- " << (char)('a' + n - 1)
           << " [style=dotted];\n";
    }
    file << "}\n";
    file.close();
  }
};

std::ostream &operator<<(std::ostream &os, Cycle &G) {
  for (int i = 0; i < G.n; i++) {
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
