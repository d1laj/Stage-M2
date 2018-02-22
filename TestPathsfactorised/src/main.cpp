#include "cycle.hpp"
//#include "path.hpp"
#include <iostream>

using namespace std;

int main() {
  cout << "Test\n";
  Cycle C(4, true);
  C.vo.test_indices();
  C.generate("test.cnf");
  C.minisat("test.cnf", "test.sol", 0);
}
