#include "path.hpp"
#include <iostream>

using namespace std;

constexpr int nmax = 16;

int main() {
  int kglob = 1;
  for (int n = 16; n <= nmax; n++) {
    Path G(n);
    int count = 0;
    int k = kglob - 1;
    Path max(n);
    while (!G.end && count < 16) {
      // cout <<"G : " <<  G;
      // count++;
      int t = G.coloring(k + 1);
      cout << '|';
      if (t > k) {
        k = t;
        kglob = k;
        max = G;
        cout << "\nk:";
        cout << k << endl;
        cout << G;
        cout << '\n';
      }
      ++G;
    }
    max.save("result/" + std::to_string(n) + ".dot");
  }
}
