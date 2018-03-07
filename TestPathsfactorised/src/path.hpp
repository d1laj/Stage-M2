#ifndef PATH_HPP
#define PATH_HPP

#include "graph.hpp"

template <bool isTwoColo> struct Path : public Graph<isTwoColo> {
  Path(int _N, int _K) : Graph<isTwoColo>(_N, _K) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    this->generate_graph_clauses(file);

    for (VertexPair vp(this->N); !vp.is_end(); vp++) {
      if (vp.v - vp.u != 1) {
        file << -this->vo.index(VariableType::Edge, vp) << End;
      } else {
        file << this->vo.index(VariableType::Edge, vp) << End;
      }
    }
    file.close();
  }

  virtual std::string name() {
    return "path_" + std::to_string(this->N) + "_" + std::to_string(this->K) +
           "_" + (isTwoColo ? "tc" : "s");
  }
};

template <bool isTwoColo> struct Param<Path<isTwoColo>> {
  int Nmax;
  int n, k;
  int timeout = 30;
  Param(int _Nmax) : Nmax(_Nmax), n(1), k(1) {}

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

  Path<isTwoColo> gen() { return Path<isTwoColo>(n, k); }
};
#endif
