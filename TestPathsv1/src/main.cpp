#include "cycle.hpp"
#include "graph.hpp"
#include "path.hpp"
#include <iostream>

using namespace std;

constexpr int nmax = 20;

void test_graph() {
  int n;
  cin >> n;
  graph::Graph G(n);
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      cout << i << " " << j << ":\n";
      cin >> G.matrix[i][j];
      G.matrix[j][i] = G.matrix[i][j];
    }
  }

  cout << "achrom: " << G.achrom() << "\n";
  cout << G << '\n';
}

int compute() {
  // int kglob = 1;
  int count = 0;
  int prediction = 7;
  for (int n = 14; n <= nmax; n++) {
    cout << "\n\n" << n << "\n\n";
    Cycle G(n);

    cout << "prediction : " << prediction << '\n';
    while (!G.end) {
      count++;
      if (!G.is_first_of_class()) {
        // cout << "b\n";
        // cout << G << '\n';
        ++G;
        continue;
      }
      // cout << G << "\n";
      if (true) {
        int k = G.achrom_signed_min(prediction);
        prediction = k + 1;
        // int l = G.achrom_signed();
        // int j = G.achrom_signed_max();
        // int l = G.get_clash_signed().achrom();
        if (true) {
          cout << G;
          cout << k << "\n\n";
          // cout << count << '\n';
        }
      } else if (false) {
        int k = G.achrom();
        G.max_achrom_moins_une_edge(k);
      }
      // cout << k << '\n';
      // cout << "------------------------\n";
      ++G;
    }
    prediction++;
    n++;
  }
  return 0;
}

int main() {
  compute();
  // test_graph();
}
