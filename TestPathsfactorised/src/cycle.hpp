#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "graph.hpp"

template <bool isTwoColo> struct Cycle : public Graph<isTwoColo> {
  bool an = false;
  Cycle(int _N, int _K, bool _an = false) : Graph<isTwoColo>(_N, _K), an(_an) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);

    for (VertexPair vp(this->N); !vp.is_end(); vp++) {
      if (vp.v - vp.u != 1 && (vp.v != this->N - 1 || vp.u != 0)) {
        this->ed[vp.index()] = 4;
      } else {
        this->ed[vp.index()] = 1;
        if (an) {
          if (vp.u % 2 == 0 && (vp.u != 0 || vp.v != this->N - 1)) {
            this->ed[vp.index()] = 2;
          } else {
            this->ed[vp.index()] = 3;
          }
        }
      }
    }

    this->generate_graph_clauses(file);

    file.close();
  }

  virtual std::string name() {
    return "cycle_" + std::to_string(this->N) + "_" + std::to_string(this->K) +
           "_" + (isTwoColo ? "tc" : "s") + (an ? "_an" : "");
  }
};

template <bool isTwoColo> struct Param<Cycle<isTwoColo>> {
  int Nmax;
  int n, k;
  int timeout = 30;
  Param(int _Nmax, int tm = 30) : Nmax(_Nmax), n(1), k(1), timeout(tm) {}

  void init() {
    n = 1;
    k = 1;
  }

  Param operator++() {
    if (k < n) {
      k++;
    } else {
      n++;
      k = 1;
    }
    return *this;
  }

  int operator[](int i) {
    if (i == 0) {
      return n;
    }
    if (i == 1) {
      return k;
    }
    return -1;
  }

  bool is_end() { return n > Nmax; }

  int nb_param() { return 2; }

  Cycle<isTwoColo> gen() { return Cycle<isTwoColo>(n, k); }
};
#endif
