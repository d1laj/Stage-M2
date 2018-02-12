#include "path.hpp"
#include <iostream>

using namespace std;

constexpr int n = 3;

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

int main() {
  // test_indices();
  for (int n = 16; n < 17; n++) {
    int kmax = 1;
    bool bigenough = false;
    for (int k = 1; k <= n; k++) {
      Path P(n, k);
      P.generate("cnfs/" + std::to_string(n) + "_" + to_string(k) + ".cnf");
      bool res = P.minisat(
          "cnfs/" + std::to_string(n) + "_" + to_string(k) + ".cnf",
          "cnfs/sol" + std::to_string(n) + "_" + to_string(k) + ".txt", 0);
      cout << n << " " << k << " " << res << endl << endl;

      if (!res && bigenough) {
        break;
      } else {
        bigenough = true;
        kmax = k;
      }
    }

    for (int i = 1; i < kmax; i++) {
      int pid = fork();
      if (pid == 0) {
        execl(
            "/bin/rm", "rm",
            ("cnfs/sol" + std::to_string(n) + "_" + to_string(i) + ".txt")
                .c_str(),
            ("cnfs/" + std::to_string(n) + "_" + to_string(i) + ".cnf").c_str(),
            (char *)0);
      }
      wait(0);
    }

    int pid = fork();
    if (pid == 0) {
      execl(
          "/bin/rm", "rm",
          ("cnfs/sol" + std::to_string(n) + "_" + to_string(kmax + 1) + ".txt")
              .c_str(),
          ("cnfs/" + std::to_string(n) + "_" + to_string(kmax + 1) + ".cnf")
              .c_str(),
          (char *)0);
    }
    wait(0);

    Path P(n, kmax);
    P.to_graph("cnfs/sol" + std::to_string(n) + "_" + to_string(kmax) + ".txt",
               "result/dot" + std::to_string(n) + "_" + to_string(kmax) +
                   ".dot");
  }
}
