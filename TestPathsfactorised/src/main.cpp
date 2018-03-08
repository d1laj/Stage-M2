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
  int nmax = 5;
  // test_cycles();
  cout << "Test\n";

  int cas = 0;
  int tm = 80;
  int N = 25;
  while (nmax <= N || tm <= 320) {
    std::cout << "nmax = " << nmax << " cas " << cas << "\n";
    int mtm = (tm == 320 ? 1281 : 1);
    switch (cas) {
    case 0: {
      CSV<Path<true>> csv(Param<Path<true>>(nmax, tm), "result/path_tc.csv",
                          mtm);
      csv.run();
    } break;
    case 1: {
      CSV<Path<false>> csv(Param<Path<false>>(nmax, tm), "result/path_s.csv",
                           mtm);
      csv.run();
    } break;

    case 2: {
      CSV<Cycle<true>> csv(Param<Cycle<true>>(nmax, tm), "result/cycle_tc.csv",
                           mtm);
      csv.run();
    } break;
    case 3: {
      CSV<Cycle<false>> csv(Param<Cycle<false>>(nmax, tm), "result/cycle_s.csv",
                            mtm);
      csv.run();
    } break;

    case 4: {
      CSV<KStarPath<true, 3>> csv(Param<KStarPath<true, 3>>(nmax, tm),
                                  "result/3star_tc.csv", mtm);
      csv.run();
    } break;
    case 5: {
      CSV<KStarPath<false, 3>> csv(Param<KStarPath<false, 3>>(nmax, tm),
                                   "result/3star_s.csv", mtm);
      csv.run();
    } break;

    case 6: {
      CSV<Clique<true>> csv(Param<Clique<true>>(nmax / 2, tm),
                            "result/clique_tc.csv", mtm);
      csv.run();
    } break;
    case 7: {
      CSV<Clique<false>> csv(Param<Clique<false>>(nmax / 2, tm),
                             "result/clique_s.csv", mtm);
      csv.run();
    } break;
    default:
      CSV<Path<true>> csv(Param<Path<true>>(15), "result/path_tc_test.csv");
      csv.run();
      break;
    }
    cas = (cas + 1) % 8;
    if (cas == 0) {
      nmax += 5;
    }
    if (nmax > N) {
      nmax = 5;
      tm *= 4;
    }
  }
  /*
  int count = 0;
  Param<KStarPath<true, 3>> p(6);
  p.init();
  while (!p.is_end() && count < 50) {
    count++;
    cout << p[0] << " " << p[1] << " ";
    for (int i = 0; i < 2; i++) {
      cout << p.legs[i] << " ";
    }
    cout << '\n';
    ++p;
  }
  */

  // p.gen().vo.test_indices();
  // p.gen().test(0, true);

  // Clique<true> C(3, 1);
  // C.vo.test_indices();
  // C.test(0, true);

  /*
  C.generate("test.cnf");
  C.minisat("test.cnf", "test.sol", 0, true);
  C.to_graph("test.sol", "test.dot");
  */
}
