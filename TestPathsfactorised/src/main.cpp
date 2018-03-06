#include "cycle.hpp"
//#include "path.hpp"
#include <iostream>

using namespace std;

auto achrom(int N) {
  std::vector<bool> val;
  for (int k = 1; k <= N; k++) {
    Cycle<false> C(N, k, false);
    C.generate("test.cnf");
    val.push_back(C.minisat("test.cnf", "test.sol", 0, false));
  }
  return val;
}

void test_cycles() {
  std::vector<std::vector<bool>> table;

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

  test_cycles();
  // cout << "Test\n";
  // Cycle<true> C(8, 6, false);
  // C.vo.test_indices();
  // C.generate("test.cnf");
  // C.minisat("test.cnf", "test.sol", 0, true);
}
