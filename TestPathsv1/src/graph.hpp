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

  Graph remove(int v, int u) {
    Graph ret(n - 1);
    auto matrixcopy = matrix;
    matrixcopy[u][v] = matrixcopy[v][u] = 0;
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

  bool is_complete_signed(int k) {
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
        bool diff = false;
        bool eq = false;
        for (int a = 0; a < n && (!diff || !eq); a++) {
          for (int b = 0; b < n && (!diff || !eq); b++) {
            if (color[a] == i && color[b] == j) {
              if (matrix[a][b] > 0) {
                diff = eq = true;
              }
              for (int c = 0; c < n && (!diff || !eq); c++) {
                for (int d = 0; d < n && (!diff || !eq); d++) {
                  if (!diff && color[c] == color[d] && matrix[a][c] > 0 &&
                      matrix[b][d] > 0 && matrix[a][c] != matrix[b][d]) {
                    diff = true;
                  }
                  if (!eq && color[c] == color[d] && matrix[a][c] > 0 &&
                      matrix[b][d] > 0 && matrix[a][c] == matrix[b][d]) {
                    eq = true;
                  }
                }
              }
            }
          }
        }
        if (!diff || !eq) {
          // std::cout << "tc: pas complet i j = " << i << j << "\n";
          return false;
        }
      }
    }

    return true;
  }

  bool has_k_color_possible(int i, int k) {
    std::vector<bool> col(k, false);
    for (int j = 0; j < i; j++) {
      col[color[j]] = true;
    }
    int count = 0;
    for (int j = 0; j < k; j++) {
      if (col[j])
        count++;
    }
    if (k - count <= n - i) {
      return true;
    }
    return false;
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
        if (has_k_color_possible(c + 1, k) && is_colo(c + 1)) {
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

  bool has_complete_signed(int k) {
    for (int i = 0; i < n; i++) {
      color[i] = 0;
    }
    bool theend = false;
    int c = 0;
    while (!theend) {
      // std::cout << *this << '\n' << c << '\n';

      if (c <= n - 1) {
        if (has_k_color_possible(c + 1, k) && is_colo(c + 1)) {
          // std::cout << "colo\n";
          c++;
          continue;
        } else {
          // std::cout << "pascolo\n";
        }
      } else {
        // std::cout << "test complete\n";
        if (is_complete_signed(k)) {
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

  void edge_switch(int i, int j) {
    matrix[i][j] = (3 - matrix[i][j]) % 3;
    matrix[j][i] = matrix[i][j];
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

  int achrom_signed_base() {
    for (int i = n; i >= 0; i--) {
      // std::cout << "-------" << i << "--------\n";
      if (has_complete_signed(i)) {
        return i;
      }
    }
    return -1;
  }

  int achrom_signed_min() {
    std::vector<bool> switching;
    for (int i = 0; i < n; i++) {
      switching.push_back(false);
    }
    int k_min = n + 1;
    bool stop = false;
    while (!stop) {
      int k = achrom();
      k_min = min(k_min, k);

      for (int i = n - 1; i >= 0; i--) {
        if (i == 0 && switching[i]) {
          stop = true;
          break;
        }
        switching[i] = !switching[i];
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
        if (switching[i]) {
          break;
        }
      }
    }

    for (int i = 0; i < n; i++) {
      if (switching[i]) {
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
      }
    }
    return k_min;
  }

  bool is_first_of_class() {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (matrix[i][j] == 0) {
          continue;
        } else if (matrix[i][j] == 1) {
          break;
        } else {
          return false;
        }
      }
    }
    return true;
  }

  int achrom_signed_max() {
    std::vector<bool> switching;
    for (int i = 0; i < n; i++) {
      switching.push_back(false);
    }
    int k_max = -1;
    bool stop = false;
    while (!stop) {
      int k = achrom();
      k_max = max(k_max, k);

      for (int i = n - 1; i >= 0; i--) {
        if (i == 0 && switching[i]) {
          stop = true;
          break;
        }
        switching[i] = !switching[i];
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
        if (switching[i]) {
          break;
        }
      }
    }

    for (int i = 0; i < n; i++) {
      if (switching[i]) {
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
      }
    }
    return k_max;
  }

  int achrom_signed() {
    std::vector<bool> switching;
    for (int i = 0; i < n; i++) {
      switching.push_back(false);
    }
    int k_max = -1;
    bool stop = false;
    while (!stop) {
      int k = achrom_signed_base();
      k_max = max(k_max, k);

      for (int i = n - 1; i >= 0; i--) {
        if (i == 0 && switching[i]) {
          stop = true;
          break;
        }
        switching[i] = !switching[i];
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
        if (switching[i]) {
          break;
        }
      }
    }

    for (int i = 0; i < n; i++) {
      if (switching[i]) {
        for (int j = 0; j < n; j++) {
          edge_switch(i, j);
        }
      }
    }
    return k_max;
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

  int max_achrom_signed_min_moins_un(int p) {
    int k = 0;
    for (int i = 0; i < n; i++) {
      Graph G1 = this->remove(i);
      int h = G1.achrom_signed_min();
      k = max(k, h);
      if (k > p) {
        std::cout << G1;
      }
      // std::cout << G1 << h << "\n";
    }
    return k;
  }

  int max_achrom_signed_max_moins_un(int p) {
    int k = 0;
    for (int i = 0; i < n; i++) {
      Graph G1 = this->remove(i);
      int h = G1.achrom_signed_max();
      k = max(k, h);
      if (k > p) {
        std::cout << G1;
      }
      // std::cout << G1 << h << "\n";
    }
    return k;
  }

  int min_achrom_signed_max_moins_un(int p) {
    int k = n - 1;
    for (int i = 0; i < n; i++) {
      Graph G1 = this->remove(i);
      int h = G1.achrom_signed_max();
      k = min(k, h);
      if (k < p - 1) {
        std::cout << G1;
      }
      // std::cout << G1 << h << "\n";
    }
    return k;
  }

  int max_achrom_moins_une_edge(int p) {
    int k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j]) {
          Graph G1 = this->remove(i, j);
          int h = G1.achrom();
          k = max(k, h);
          if (k > p + 1) {
            std::cout << G1;
          }
        }
        // std::cout << G1 << h << "\n";
      }
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

  void add_edge(int i, int j, bool positive) {
    matrix[i][j] = (positive ? 1 : 2);
    matrix[j][i] = matrix[i][j];
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

  Graph get_clash() {
    Graph C(n);
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j]) {
          C.add_edge(i, j, true);
        } else {
          for (int k = 0; k < n; k++) {
            if (k != i && k != j && matrix[i][k] && matrix[j][k] &&
                matrix[i][k] != matrix[j][k]) {
              C.add_edge(i, j, true);
              break;
            }
          }
        }
      }
    }
    return C;
  }

  Graph get_clash_signed() {
    Graph C(n);
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        if (matrix[i][j]) {
          C.add_edge(i, j, true);
        } else {
          bool stop = false;
          for (int k = 0; k < n && !stop; k++) {
            if (k != i && k != j && matrix[i][k] && matrix[j][k] &&
                matrix[i][k] != matrix[j][k]) {
              for (int l = 0; l < n && !stop; l++) {
                if (l != i && l != j && l != k && matrix[i][l] &&
                    matrix[j][l] && matrix[i][l] == matrix[j][l]) {
                  C.add_edge(i, j, true);
                  stop = true;
                }
              }
            }
          }
        }
      }
    }
    return C;
  }
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
