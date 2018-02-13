#include "cycle.hpp"
#include "path.hpp"
#include <iostream>

using namespace std;

constexpr int nmax = 19;

int main() {
  // int kglob = 1;
  for (int n = 19; n <= nmax; n++) {
    cout << "\n\n\n";
    Path G(n);
    int count = 0;
    // int k = 1;
    Path max(n);
    int min_k = n * n;
    while (!G.end && count < 16) {
      // cout <<"G : " <<  G;
      // count++;
      int t = G.coloring(4);
      if (t < min_k) {
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
      ++G;
    }
    cout << "n=" << n << " et min_k=" << min_k << "\n";
    max.save("result/" + std::to_string(n) + ".dot");
  }
}
