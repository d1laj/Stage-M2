#include "clique.hpp"
#include "cycle.hpp"
#include "fill_csv.hpp"
#include "kstar.hpp"
#include "path.hpp"
#include <iostream>

using namespace std;

auto achrom(int N) {
  std::vector<TernaryBoolean> val;
  for (int k = 1; k <= N; k++) {
    Cycle<false> C(N, k, false);
    C.generate("test.cnf");
    val.push_back(C.minisat("test.cnf", "test.sol", 0, false));
  }
  return val;
}

void test_cycles() {
  std::vector<std::vector<TernaryBoolean>> table;

  for (int n = 3; n < 15; n++) {
    table.push_back(achrom(n));

    cout << "-----------  " << n << "  ---------------\n";
    for (auto &t : table) {
      for (auto v : t) {
        cout << v << " ";
      }
      cout << '\n';
    }
  }
}

int main() {

  // test_cycles();
  cout << "Test\n";

  // CSV<Cycle<false>> csv(Param<Cycle<false>>(25), "result/cycle_s.csv");
  cout << "Test\n";
  // csv.run();
  int count = 0;
  Param<KStarPath<true, 3>> p(5);
  p.init();
  while (!p.is_end() && count < 20) {
    count++;
    for (int i = 0; i < 5; i++) {
      cout << p[i] << " ";
    }
    cout << '\n';
    ++p;
  }

  // Clique<true> C(3, 1);
  // C.vo.test_indices();
  // C.test(0, true);

  /*
  C.generate("test.cnf");
  C.minisat("test.cnf", "test.sol", 0, true);
  C.to_graph("test.sol", "test.dot");
  */
}
