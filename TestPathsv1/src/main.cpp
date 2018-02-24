#include "cycle.hpp"
#include "graph.hpp"
#include "path.hpp"
#include <iostream>

using namespace std;

constexpr int nmax = 10;

int main() {
  // int kglob = 1;
  for (int n = 4; n <= nmax; n++) {
    cout << "\n\n" << n << "\n\n";
    graph::Graph G(n);

    while (!G.end) {
      int k = G.achrom();
      int l = G.max_achrom_moins_un(k);
      if (k < l) {
        cout << G;
        cout << k << " " << l << "\n\n";
      }
      // cout << k << '\n';
      // cout << "------------------------\n";
      ++G;
    }

    // int count = 0;
    // int k = 1;
    // Cycle max(n);
    // int min_k = n * n;
    // while (!G.end && count < 16) {
    // cout <<"G : " <<  G;
    // count++;
    // int t = G.coloring();
    /*
    if (t < min_k && false) {
      min_k = t;
      // kglob = k;
      max = G;
      cout << "\n" << n << " phy:";
      cout << t << endl;
      cout << G;
      cout << '\n';
    } else if (false) {
      cout << "\n" << n << " phy:";
      cout << t << endl;
      cout << G;
      cout << '\n';
    }
    */
    //++G;
    //}
    // cout << "n=" << n << " et k=" << t << "\n";
    // cout << G << "\n";
    // G.save("result/an_" + std::to_string(n) + ".dot");
  }
}
