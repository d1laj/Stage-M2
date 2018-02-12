#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <array>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

Literal edge_id(int n, int i, int j, int sign) {
  return 2 * alpha(n, i, j) + sign + 1;
}
Literal W_id(int n, int i, int j, int k) {
  return 1 + n * (n - 1) + alpha(n, i, j) * (n - 2) +
         (k < min(i, j) ? k : (k > max(i, j) ? k - 2 : k - 1));
}
Literal M_id(int n, int m, int i, int j, int k) {
  return 1 + n * (n - 1) + n * (n - 1) / 2 * (n - 2) +
         alpha(n, i, j) * (m + 1) + k;
}
Literal EvenDeg(int n, int m, int i, int k) {
  return 1 + n * (n - 1) + n * (n - 1) / 2 * (n - 2) +
         n * (n - 1) / 2 * (m + 1) + i * (n - 1) + (k < i ? k : k - 1);
}

struct Graph {
  int n;
  int m;
  bool p, c;
  Graph(int _n, int _m, bool _p, bool _c) : n(_n), m(_m), p(_p), c(_c) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);

    file << "p cnf 0 0\n";

    file << "c help to minisat to remove posibilities\n";
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        file << edge_id(n, i, j, 0) << edge_id(n, i, j, 1) << End;
      }
    }

    file << "c proper colo\n";

    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        file << edge_id(n, i, j, 0);
        for (int l = 0; l < n; l++) {
          if (l != i && l != j) {
            file << W_id(n, i, j, l);
          }
        }
        file << End;
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        for (int l = 0; l < n; l++) {
          if (l != i && l != j) {
            file << -W_id(n, i, j, l) << edge_id(n, i, l, 0) << End;
            file << -W_id(n, i, j, l) << edge_id(n, j, l, 0) << End;
            file << -W_id(n, i, j, l) << edge_id(n, i, l, 1)
                 << edge_id(n, j, l, 1) << End;
            file << -W_id(n, i, j, l) << -edge_id(n, i, l, 1)
                 << -edge_id(n, j, l, 1) << End;

            file << W_id(n, i, j, l) << -edge_id(n, i, l, 0)
                 << -edge_id(n, j, l, 0) << edge_id(n, i, l, 1)
                 << -edge_id(n, j, l, 1) << End;
            file << W_id(n, i, j, l) << -edge_id(n, i, l, 0)
                 << -edge_id(n, j, l, 0) << -edge_id(n, i, l, 1)
                 << edge_id(n, j, l, 1) << End;
          }
        }
      }
    }

    file << "c Graph as few edges\n";

    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        int i_prev, j_prev;
        if (j > i + 1) {
          i_prev = i;
          j_prev = j - 1;
        } else if (i > 0) {
          i_prev = i - 1;
          j_prev = n - 1;
        } else {
          continue;
        }
        for (int k = 0; k <= m; k++) {
          file << -M_id(n, m, i_prev, j_prev, k) << M_id(n, m, i, j, k) << End;
          file << -M_id(n, m, i, j, k) << M_id(n, m, i_prev, j_prev, k)
               << edge_id(n, i, j, 0) << End;
          if (k > 0) {
            file << -M_id(n, m, i_prev, j_prev, k - 1) << -edge_id(n, i, j, 0)
                 << M_id(n, m, i, j, k) << End;
            file << -M_id(n, m, i, j, k) << M_id(n, m, i_prev, j_prev, k)
                 << M_id(n, m, i_prev, j_prev, k - 1) << End;
          }
        }
      }
    }
    file << M_id(n, m, 0, 1, 0) << End;
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        file << -M_id(n, m, i, j, m) << End;
      }
    }
    file << M_id(n, m, n - 2, n - 1, m - 1) << End;

    if (p || c) {
      file << "c Variable for even degree\n";

      for (int i = 0; i < n; i++) {
        file << -EvenDeg(n, m, i, 0) << edge_id(n, i, 0, 0) << End;
        file << EvenDeg(n, m, i, 0) << -edge_id(n, i, 0, 0) << End;
        int kprev = 0;
        for (int k = 1; k < n; k++) {
          if (k != i) {
            file << -EvenDeg(n, m, i, k) << -edge_id(n, i, k, 0)
                 << -EvenDeg(n, m, i, kprev) << End;
            file << -EvenDeg(n, m, i, k) << edge_id(n, i, k, 0)
                 << EvenDeg(n, m, i, kprev) << End;
            file << EvenDeg(n, m, i, k) << edge_id(n, i, k, 0)
                 << -EvenDeg(n, m, i, kprev) << End;
            file << EvenDeg(n, m, i, k) << -edge_id(n, i, k, 0)
                 << EvenDeg(n, m, i, kprev) << End;
            kprev = k;
          }
        }
      }
    }

    if (p) {
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          for (int k = j + 1; k < n; k++) {
            file << -EvenDeg(n, m, i, (i == n - 1 ? n - 2 : n - 1));
            file << -EvenDeg(n, m, j, (j == n - 1 ? n - 2 : n - 1));
            file << -EvenDeg(n, m, k, (k == n - 1 ? n - 2 : n - 1)) << End;
          }
        }
      }
    }
    if (c) {
      for (int i = 0; i < n; i++) {
        file << -EvenDeg(n, m, i, (i == n - 1 ? n - 2 : n - 1)) << End;
      }
    }

    file.close();
  }

  bool minisat(std::string filename, std::string outfile, int timeout) {
    int pid = fork();
    if (pid == 0) {
      execl("./glucose_static", "glucose_static", "-rnd-init", "-rnd-freq=0.1",
            filename.c_str(), outfile.c_str(), (char *)0);
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
      kill(pid, SIGINT);
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

    std::ofstream ofile;
    ofile.open(outfile);
    ofile << "graph {" << '\n';

    int val;
    for (int i = 0; i < n; i++) {
      for (int j = i + 1; j < n; j++) {
        ifile >> val;
        if (val > 0) {
          ifile >> val;
          if (val > 0) {
            ofile << "a" + std::to_string(i) + " -- " + "a" + std::to_string(j)
                  << ";\n";
          } else {
            ofile << "a" + std::to_string(i) + " -- " + "a" + std::to_string(j)
                  << " [style=dotted];\n";
          }
        } else {
          ifile >> val;
        }
      }
    }

    for (int i = 0; i < n; i++) {
      ofile << "a" + std::to_string(i) << "[label=" + std::to_string(i)
            << "];\n";
    }

    ofile << "}\n";
    ofile.close();
    ifile.close();
  }
};

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
