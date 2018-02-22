#ifndef PATH_HPP
#define PATH_HPP

#include <algorithm>
#include <array>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace path {
inline int min(int a, int b) { return (a < b) ? a : b; }
inline int max(int a, int b) { return (a > b) ? a : b; }

inline int alpha(int n, int a, int b) {
  if (b < a) {
    return alpha(n, b, a);
  } else {
    return a * n + b - (a + 1) * (a + 2) / 2;
  }
}

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

std::string End = "0\n";

Literal edge_id(int u) { return u + 1; }
Literal vertex_id(int n, int K, int u, int i) { return n + u * K + i; }
Literal hom_edge_id(int n, int K, int i, int j, int sign) {
  return n + K * (n - 1) + K + 2 * alpha(K, i, j) + sign;
}
Literal X_id(int n, int K, int u, int i, int j, int sign) {
  return n + K * n + K * (K - 1) + 2 * u * K * (K - 1) +
         2 * (i * (K - 1) + j + (j < i ? 0 : -1)) + sign;
}
Literal W_id(int n, int K, int i, int j, int l) {
  return n + K * n + K * (K - 1) + 2 * (n - 1) * K * (K - 1) +
         alpha(K, i, j) * (K - 2) +
         (l < min(i, j) ? l : (l > max(i, j) ? l - 2 : l - 1));
}

struct Path {
  int n;
  int K;
  Path(int _n, int _K) : n(_n), K(_K) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);

    file << "p cnf 0 0\n";

    file << "c K couleurs\n";

    for (int i = 0; i < K; i++) {
      for (int u = 0; u < n; u++) {
        file << vertex_id(n, K, u, i);
      }
      file << End;
    }

    file << edge_id(0) << End;
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < K; j++) {
        file << -vertex_id(n, K, i, j) << End;
      }
    }

    // G is colored
    file << "c 1 color par vertex et au plus 1\n";
    for (int u = 0; u < n; u++) {
      for (int i = 0; i < K; i++) {
        file << vertex_id(n, K, u, i);
      }
      file << End;
      for (int i = 0; i < K; i++) {
        for (int j = i + 1; j < K; j++) {
          file << -vertex_id(n, K, u, j) << -vertex_id(n, K, u, i) << End;
        }
      }
    }

    // Gc coresp to G
    file << "c gc coresp to G\n";
    for (int u = 0; u < n - 1; u++) {
      for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
          if (i != j) {
            file << -vertex_id(n, K, u, i) << -vertex_id(n, K, u + 1, j)
                 << hom_edge_id(n, K, i, j, 0) << End;
            file << -vertex_id(n, K, u, i) << -vertex_id(n, K, u + 1, j)
                 << -hom_edge_id(n, K, i, j, 1) << edge_id(u) << End;
            file << -vertex_id(n, K, u, i) << -vertex_id(n, K, u + 1, j)
                 << hom_edge_id(n, K, i, j, 1) << -edge_id(u) << End;
          }
        }
      }
    }

    // X
    file << "c X\n";
    for (int u = 0; u < n - 1; u++) {
      for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
          if (i != j) {
            file << -X_id(n, K, u, i, j, 1) << vertex_id(n, K, u, i) << End;
            file << -X_id(n, K, u, i, j, 0) << vertex_id(n, K, u, i) << End;
            file << -X_id(n, K, u, i, j, 1) << vertex_id(n, K, u + 1, j) << End;
            file << -X_id(n, K, u, i, j, 0) << vertex_id(n, K, u + 1, j) << End;
            file << -X_id(n, K, u, i, j, 1) << edge_id(u) << End;
            file << -X_id(n, K, u, i, j, 0) << -edge_id(u) << End;

            file << X_id(n, K, u, i, j, 1) << -vertex_id(n, K, u, i)
                 << -vertex_id(n, K, u + 1, j) << -edge_id(u) << End;
            file << X_id(n, K, u, i, j, 0) << -vertex_id(n, K, u, i)
                 << -vertex_id(n, K, u + 1, j) << edge_id(u) << End;
          }
        }
      }
    }

    file << "c x suite\n";
    for (int i = 0; i < K; i++) {
      for (int j = i + 1; j < K; j++) {

        file << -hom_edge_id(n, K, i, j, 0) << -hom_edge_id(n, K, i, j, 1);
        for (int u = 0; u < n - 1; u++) {
          file << X_id(n, K, u, i, j, 1) << X_id(n, K, u, j, i, 1);
        }
        file << End;
      }
    }
    for (int i = 0; i < K; i++) {
      for (int j = i + 1; j < K; j++) {

        file << -hom_edge_id(n, K, i, j, 0) << hom_edge_id(n, K, i, j, 1);
        for (int u = 0; u < n - 1; u++) {
          file << X_id(n, K, u, i, j, 0) << X_id(n, K, u, j, i, 0);
        }
        file << End;
      }
    }

    file << "c proper colo\n";
    for (int u = 0; u < n - 1; u++) {
      for (int i = 0; i < K; i++) {
        file << -vertex_id(n, K, u, i) << -vertex_id(n, K, u + 1, i) << End;
      }
    }

    for (int i = 0; i < K; i++) {
      for (int j = i + 1; j < K; j++) {
        file << hom_edge_id(n, K, i, j, 0);
        for (int l = 0; l < K; l++) {
          if (l != i && l != j) {
            file << W_id(n, K, i, j, l);
          }
        }
        file << End;
      }
    }

    for (int i = 0; i < K; i++) {
      for (int j = i + 1; j < K; j++) {
        for (int l = 0; l < K; l++) {
          if (l != i && l != j) {
            file << -W_id(n, K, i, j, l) << hom_edge_id(n, K, i, l, 0) << End;
            file << -W_id(n, K, i, j, l) << hom_edge_id(n, K, j, l, 0) << End;
            file << -W_id(n, K, i, j, l) << hom_edge_id(n, K, i, l, 1)
                 << hom_edge_id(n, K, j, l, 1) << End;
            file << -W_id(n, K, i, j, l) << -hom_edge_id(n, K, i, l, 1)
                 << -hom_edge_id(n, K, j, l, 1) << End;

            file << W_id(n, K, i, j, l) << -hom_edge_id(n, K, i, l, 0)
                 << -hom_edge_id(n, K, j, l, 0) << hom_edge_id(n, K, i, l, 1)
                 << -hom_edge_id(n, K, j, l, 1) << End;
            file << W_id(n, K, i, j, l) << -hom_edge_id(n, K, i, l, 0)
                 << -hom_edge_id(n, K, j, l, 0) << -hom_edge_id(n, K, i, l, 1)
                 << hom_edge_id(n, K, j, l, 1) << End;
          }
        }
      }
    }

    file.close();
  }

  bool minisat(std::string filename, std::string outfile, int timeout) {
    int pid = fork();
    if (pid == 0) {
      execl("./minisat", "minisat", "-rnd-freq=0.01", filename.c_str(),
            outfile.c_str(), (char *)0);
    }
    int status;
    int ret = waitpid(pid, &status, WNOHANG);
    int k = 0;
    while (!ret && (k < timeout || timeout == 0)) {
      k++;
      sleep(1);
      ret = waitpid(pid, &status, WNOHANG);
    }
    if (!ret) {
      kill(pid, SIGTERM);
      wait(0);
      return false;
    }
    wait(0);

    std::ifstream file;
    file.open(outfile);
    std::string firstword;
    file >> firstword;
    file.close();

    if (firstword == "UNSAT") {
      return false;
    }
    return true;
  }

  void to_graph(std::string filename, std::string outfile) {
    std::ifstream ifile;
    ifile.open(filename);
    std::string firstword;
    ifile >> firstword;

    std::ofstream ofile;
    ofile.open(outfile);
    ofile << "graph {" << '\n';

    int val;
    for (int i = 0; i < n - 1; i++) {
      ifile >> val;
      if (val > 0) {
        ofile << "a" + std::to_string(i) + " -- " + "a" + std::to_string(i + 1)
              << ";\n";
      } else {
        ofile << "a" + std::to_string(i) + " -- " + "a" + std::to_string(i + 1)
              << " [style=dotted];\n";
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < K; j++) {
        ifile >> val;
        if (val > 0) {
          ofile << "a" + std::to_string(i) << "[label=" + std::to_string(j)
                << "];\n";
        }
      }
    }

    ofile << "}\n";
    ofile.close();
    ifile.close();
  }
};
} // namespace path
/*
void save(std::string filename) {
    std::ofstream file(filename);

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

*/
#endif
