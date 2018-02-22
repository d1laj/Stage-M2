#include "cycle.hpp"
//#include "path.hpp"
#include <iostream>

using namespace std;

int main() {
  cout << "Test\n";
  Cycle C(6, true);

  C.generate("test.cnf");
  C.minisat("test.cnf", "test.sol", 0);
}
