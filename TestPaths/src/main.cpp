#include "cycle.hpp"
#include "path.hpp"
#include <iostream>

using namespace std;
using namespace cycle;

constexpr int n = 4;

int test_indices() {
  int K = 3;
  for (int u = 0; u < n; u++) {
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

  for (int u = 0; u < n; u++) {
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

int main() {
  // test_indices();
  for (int n = 4; n < 55; n += 2) {
    int kmax = 1;
    for (int k = 1; k <= n; k++) {
      cycle::Cycle P(n, k, true);
      P.generate("cnfs/an" + std::to_string(n) + "_" + to_string(k) + ".cnf");
      bool res = P.minisat(
          "cnfs/an" + std::to_string(n) + "_" + to_string(k) + ".cnf",
          "cnfs/solan" + std::to_string(n) + "_" + to_string(k) + ".txt", 60);
      cout << n << " " << k << " " << res << endl << endl;

      if (res) {
        kmax = k;
      }
    }

    for (int i = 1; i <= n; i++) {
      if (i != kmax && true) {
        int pid = fork();
        if (pid == 0) {
          execl("/bin/rm", "rm",
                ("cnfs/solan" + std::to_string(n) + "_" + to_string(i) + ".txt")
                    .c_str(),
                ("cnfs/an" + std::to_string(n) + "_" + to_string(i) + ".cnf")
                    .c_str(),
                (char *)0);
        }
        wait(0);
      }
    }

    cycle::Cycle P(n, kmax, true);
    P.to_graph(
        "cnfs/solan" + std::to_string(n) + "_" + to_string(kmax) + ".txt",
        "result/dotan" + std::to_string(n) + "_" + to_string(kmax) + ".dot");
  }
}
