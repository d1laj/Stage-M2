#include "graph.hpp"
#include <cmath>
#include <iostream>

using namespace std;

int h(int n) {
  int tmp_n = n;
  int logt = 0;
  int t = 1;
  while (tmp_n >>= 1) {
    ++logt;
    t <<= 1;
  }
  return (n + 1) * logt - 2 * t + 2;
}

constexpr int timeout = 0;
constexpr int nmax = 15;
constexpr bool test_path = false;
constexpr bool test_cycle = true;

std::string make_name(int n, int m, bool underscore) {
  std::string type = "g";
  if (test_path) {
    type = "p";
  } else if (test_cycle) {
    type = "c";
  }
  if (underscore) {
    return "_" + type + to_string(n) + "_" + to_string(m);
  } else {
    return type + to_string(n) + "_" + to_string(m);
  }
}

int main(int argc, char **argv) {
  static_assert(!test_path || !test_cycle, "Cannot activate path and cycle");

  for (int n = 2; n < nmax; n++) {
    int mmin = h(n);
    for (int m = h(n) - 1; m <= n * (n - 1) / 2; m++) {
      Graph P(n, m, test_path, test_cycle);
      P.generate("cnfs/" + make_name(n, m, false) + ".cnf");
      bool res =
          P.minisat("cnfs/" + make_name(n, m, false) + ".cnf",
                    "cnfs/sol" + make_name(n, m, true) + ".txt", timeout);
      cout << n << " " << m << " " << res << endl << endl;

      if (res) {
        mmin = m;
        break;
      } else {
      }
    }

    for (int i = h(n) - 1; i < mmin; i++) {
      int pid = fork();
      if (pid == 0) {
        execl("/bin/rm", "rm",
              ("cnfs/sol" + make_name(n, i, true) + ".txt").c_str(),
              ("cnfs/" + make_name(n, i, false) + ".cnf").c_str(), (char *)0);
      }
      wait(0);
    }
    if (false) {
      int pid = fork();
      if (pid == 0) {
        execl("/bin/rm", "rm",
              ("cnfs/sol" + make_name(n, mmin - 1, true) + ".txt").c_str(),
              ("cnfs/" + make_name(n, mmin - 1, false) + ".cnf").c_str(),
              (char *)0);
      }
      wait(0);
    }
    Graph P(n, mmin, test_path, test_cycle);
    P.to_graph("cnfs/sol" + make_name(n, mmin, true) + ".txt",
               "result/" + make_name(n, mmin, false) + ".dot");
  }
}

/*
int test_indices() {
  int K = 3;
  for (int u = 0; u < n - 1; u++) {
    cout << edge_id(u);
  }
  cout << '\n';
  for (int v = 0; v < n; v++) {
    for (int i = 0; i < K; i++) {
      cout << vertex_id(n, K, v, i);
    }
    cout << '\n';
  }

  for (int i = 0; i < K; i++) {
    for (int j = i + 1; j < K; j++) {
      cout << hom_edge_id(n, K, i, j, 0) << hom_edge_id(n, K, i, j, 1) << '\n';
    }
  }
  cout << '\n';

  for (int u = 0; u < n - 1; u++) {
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < K; j++) {
        if (i != j) {
          cout << X_id(n, K, u, i, j, 0) << X_id(n, K, u, i, j, 1) << '\n';
        } else {
          cout << "=\n";
        }
      }
    }
    cout << "---\n";
  }
  cout << "\n\n";
  for (int i = 0; i < K; i++) {
    for (int j = i + 1; j < K; j++) {
      for (int l = 0; l < K; l++) {
        if (l != j && l != i) {
          cout << W_id(n, K, i, j, l) << '\n';
        } else {
          cout << "=\n";
        }
      }
    }
  }
  return 0;
}
*/
