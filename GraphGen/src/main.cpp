#include "graph.hpp"
#include <iostream>

using namespace std;

constexpr int n = 7;
constexpr int min_m = 9;
constexpr int max_m = 10;

int main() {
  Graph<n> G;
  int k = 0;
  int tour = 0;
  int m_min = n * n;
  while (!G.end) {
    tour++;
    if (tour % 100000000 == 0) {
      cout << tour << '\n';
      cout << G << "\n";
      // cout << G.is_clique() << " " << G.is_first() << "\n";
      // cout << G.edges() << endl << endl;
    }
    /*
    if (G.edges() < 13 || G.edges() > 15) {
      ++G;
      continue;
    }*/

    if (G.is_clique() && G.is_first()) {
      int m = G.edges();
      m_min = (m < m_min ? m : m_min);
      cout << '(' << m << "," << m_min << ')' << '\n';
      if (true /*G.is_clique() && G.is_first() */
          /*&& !G.is_complete_diff_of_kn()*/) {
        cout << G << std::endl;
        int m = G.edges();

        // cout << m << endl;
        G.save("colo2/result_" + std::to_string(n) + "/" + std::to_string(m) +
               "_" + std::to_string(k) + ".dot");

        k++;
      }
    }
    G.next(min_m, max_m);
  }
  cout << m_min;
}
