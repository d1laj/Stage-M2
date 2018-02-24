#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

namespace graph {

inline int min(int a, int b) { return (a < b) ? a : b; }
inline int max(int a, int b) { return (a > b) ? a : b; }
inline int mod(int x, int m) { return (x < 0 ? ((x % m) + m) % m : x % m); }

struct Graph;
std::ostream &operator<<(std::ostream &os, Graph &G);

struct Graph {
  int n;
  std::vector<std::vector<int>> matrix;
  std::vector<int> color;
  bool end = false;
  int lead = n * n - 1;
  int nb_edges = 0;

  Graph(int _n) : n(_n) {
    matrix.resize(n);
    for (int i = 0; i < n; i++) {
      matrix[i].resize(n);
      for (int j = 0; j < n; j++) {
        matrix[i][j] = 0;
      }
    }
    color.resize(n);
    for (int i = 0; i < n; i++) {
      color[i] = 0;
    }
  }

  Graph &operator++() {
    for (int i = n * n - 1; i >= 0; i--) {
      // std::cout << "c" << i << '\n';
      if (i == 0) {
        end = true;
        break;
      }
      if (i / n <= i % n) {
        continue;
      }
      lead = (lead < i) ? lead : i;
      if (i == lead && matrix[i / n][i % n] == 1) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        nb_edges--;
        continue;
      }
      matrix[i / n][i % n]++;
      matrix[i % n][i / n]++;
      nb_edges++;
      if (matrix[i / n][i % n] > 2) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        nb_edges -= 2;
        if (i == 1) {
          end = true;
        }
      } else if (matrix[i / n][i % n] == 2) {
        nb_edges--;
        break;
      } else {
        break;
      }
    }
    return *this;
  }

  // ++G but with min/max edges
  Graph &next(const int a, const int b) {
    bool all_one_before = true;
    for (int i = n * n - 1; i >= 0; i--) {
      // std::cout << "c" << i << '\n';
      if (i == 0) {
        end = true;
        break;
      }
      if (i / n <= i % n) {
        continue;
      }
      lead = (lead < i) ? lead : i;
      if (i == lead && matrix[i / n][i % n] == 1) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        all_one_before = false;
        nb_edges--;
        // std::cout << "lead" << i << '\n';
        continue;
      }
      if (nb_edges > b) {
        if (matrix[i / n][i % n]) {
          matrix[i / n][i % n] = 0;
          matrix[i % n][i / n] = 0;
          all_one_before = false;
          nb_edges--;
          // std::cout << "too much" << i << '\n';
          continue;
        } else {
          continue;
        }
      }
      if (nb_edges >= b) {
        if (!matrix[i / n][i % n]) {
          matrix[i / n][i % n] = 1;
          matrix[i % n][i / n] = 1;
          nb_edges++;
          // std::cout << "almost too much" << i << '\n';
          i = n * n - 1;
          continue;
        }
      }

      matrix[i / n][i % n]++;
      matrix[i % n][i / n]++;
      nb_edges++;
      if (matrix[i / n][i % n] > 2) {
        matrix[i / n][i % n] = 0;
        matrix[i % n][i / n] = 0;
        all_one_before = false;
        nb_edges -= 2;
        if (i == 1) {
          end = true;
        }
        // std::cout << "2->0" << i << '\n';
        continue;
      } else if (matrix[i / n][i % n] == 2) {
        all_one_before = false;
        nb_edges--;
        // std::cout << "1->2" << i << '\n';
      }
      if (nb_edges < a && !all_one_before) {
        all_one_before = true;
        i = n * n - 1;
        // std::cout << "c1" << i << '\n';
        continue;
      } else if (nb_edges < a && all_one_before) {
        // std::cout << "c2" << i << '\n';
        continue;
      }
      // std::cout << "break" << i << '\n';
      break;
    }
    return *this;
  }

  Graph remove(int v) {
    Graph ret(n - 1);
    auto matrixcopy = matrix;
    for (int i = 0; i < n; i++) {
      if (i != v) {
        matrixcopy[i].erase(matrixcopy[i].begin() + v);
      }
    }
    matrixcopy.erase(matrixcopy.begin() + v);

    ret.matrix = matrixcopy;
    return ret;
  }

  bool is_colo(int p) {
    for (int i = 0; i < p; i++) {
      for (int j = 0; j < p; j++) {
        if (i != j) {
          if (color[i] == color[j] && matrix[i][j] > 0) {
            return false;
          }
        }
      }
    }

    for (int i = 0; i < p; i++) {
      for (int j = 0; j < p; j++) {
        for (int k = 0; k < p; k++) {
          for (int l = 0; l < p; l++) {
            if (i != j && k != l && color[i] == color[k] &&
                color[j] == color[l] && matrix[i][j] > 0 && matrix[k][l] > 0 &&
                matrix[i][j] != matrix[k][l]) {
              return false;
            }
          }
        }
      }
    }

    return true;
  }

  bool is_complete(int k) {
    if (!is_colo(n)) {
      // std::cout << "tc: pas colo\n";
      return false;
    }

    for (int i = 0; i < k; i++) {
      bool found = false;
      for (int j = 0; j < n; j++) {
        if (color[j] == i) {
          found = true;
        }
      }
      if (!found) {
        // std::cout << "tc: pas color i = " << i << "\n";
        return false;
      }
    }

    for (int i = 0; i < k; i++) {
      for (int j = 0; j < k; j++) {
        if (i == j) {
          continue;
        }
        bool comp = false;
        for (int a = 0; a < n && !comp; a++) {
          for (int b = 0; b < n && !comp; b++) {
            if (color[a] == i && color[b] == j) {
              if (matrix[a][b] > 0) {
                comp = true;
              }
              for (int c = 0; c < n && !comp; c++) {
                for (int d = 0; d < n && !comp; d++) {
                  if (color[c] == color[d] && matrix[a][c] > 0 &&
                      matrix[b][d] > 0 && matrix[a][c] != matrix[b][d]) {
                    comp = true;
                  }
                }
              }
            }
          }
        }
        if (!comp) {
          // std::cout << "tc: pas complet i j = " << i << j << "\n";
          return false;
        }
      }
    }

    return true;
  }

  bool has_complete(int k) {
    for (int i = 0; i < n; i++) {
      color[i] = 0;
    }
    bool theend = false;
    int c = 0;
    while (!theend) {
      // std::cout << *this << '\n' << c << '\n';

      if (c <= n - 1) {
        if (is_colo(c + 1)) {
          // std::cout << "colo\n";
          c++;
          continue;
        } else {
          // std::cout << "pascolo\n";
        }
      } else {
        // std::cout << "test complete\n";
        if (is_complete(k)) {
          // std::cout << "complete\n";
          return true;
        }
      }
      for (int i = min(c, n - 1); i >= 0; i--) {
        if (i == 0) {
          theend = true;
          break;
        }
        c = min(i, c);
        color[i]++;
        if (color[i] == k) {
          color[i] = 0;
        } else if (color[i] > i) {
          color[i] = 0;
        } else {
          break;
        }
      }
    }
    return false;
  }

  int achrom() {
    for (int i = n; i >= 0; i--) {
      // std::cout << "-------" << i << "--------\n";
      if (has_complete(i)) {
        return i;
      }
    }
    return -1;
  }

  int max_achrom_moins_un(int p) {
    int k = 0;
    for (int i = 0; i < n; i++) {
      Graph G1 = this->remove(i);
      int h = G1.achrom();
      k = max(k, h);
      if (k > p) {
        std::cout << G1;
      }
      // std::cout << G1 << h << "\n";
    }
    return k;
  }

  bool operator==(Graph &g2) {
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] != g2.matrix[i][j]) {
          return false;
        }
      }
    }
    return true;
  }

  void save(std::string filename) {
    std::ofstream file(filename);
    file << "graph {" << '\n';
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j] == 1) {
          file << (char)('a' + i) << " -- " << (char)('a' + j) << ";\n";
        } else if (matrix[i][j] == 2) {
          file << (char)('a' + i) << " -- " << (char)('a' + j)
               << " [style=dotted];\n";
        }
      }
    }
    file << "}\n";
    file.close();
  }

  int edges() { return nb_edges; }
};

std::ostream &operator<<(std::ostream &os, Graph &G) {
  for (int i = 0; i < G.n; i++) {
    for (int j = 0; j < G.n; j++) {
      os << G.matrix[i][j] << " ";
    }
    os << "\n";
  }

  os << "c: ";
  for (int j = 0; j < G.n; j++) {
    os << G.color[j] << " ";
  }
  os << '\n';
  return os;
}

} // namespace graph
#endif
