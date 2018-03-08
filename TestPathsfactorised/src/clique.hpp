#ifndef CLIQUE_HPP
#define CLIQUE_HPP

#include "graph.hpp"

template <bool isTwoColo> struct Clique : public Graph<isTwoColo, true> {
  int M;
  Clique(int _N, int _M) : Graph<isTwoColo, true>(_N, _N), M(_M) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    this->generate_graph_clauses(file);
    generate_nb_edges_rules(file);

    file << this->vo.index(VariableType::NbEdges,
                           VertexPair(this->N, this->N - 2, this->N - 1), M)
         << End;
    file.close();
  }

  void generate_nb_edges_rules(std::ofstream &file) {

    file << "c ------------------------\nc clique conditions\n";
    for (VertexPair vp(this->N); !vp.is_end(); vp++) {
      if (vp.index() == 0) {
        file << -this->vo.index(VariableType::NbEdges, vp, 0)
             << -this->vo.index(VariableType::Edge, vp) << End;
        file << this->vo.index(VariableType::NbEdges, vp, 0)
             << this->vo.index(VariableType::Edge, vp) << End;

        for (int i = 1; i <= M; i++) {
          file << this->vo.index(VariableType::NbEdges, vp, i) << End;
        }
      } else {
        for (int i = 0; i <= M; i++) {
          if (i > 0) {
            file << -this->vo.index(VariableType::NbEdges, vp, i - 1)
                 << this->vo.index(VariableType::NbEdges, vp, i) << End;
          }
          file << -this->vo.index(VariableType::NbEdges, vp.prev(), i)
               << this->vo.index(VariableType::Edge, vp)
               << this->vo.index(VariableType::NbEdges, vp, i) << End;

          if (i > 0) {
            file << this->vo.index(VariableType::NbEdges, vp.prev(), i - 1)
                 << this->vo.index(VariableType::NbEdges, vp.prev(), i)
                 << -this->vo.index(VariableType::NbEdges, vp, i) << End;

            file << this->vo.index(VariableType::NbEdges, vp.prev(), i - 1)
                 << -this->vo.index(VariableType::Edge, vp)
                 << -this->vo.index(VariableType::NbEdges, vp, i) << End;
          } else {
            file << this->vo.index(VariableType::NbEdges, vp.prev(), i)
                 << -this->vo.index(VariableType::NbEdges, vp, i) << End;

            file << -this->vo.index(VariableType::Edge, vp)
                 << -this->vo.index(VariableType::NbEdges, vp, i) << End;
          }
        }
      }
    }
  }

  virtual std::string name() {
    return "clique_" + std::to_string(this->N) + "_" + std::to_string(this->M) +
           "_" + (isTwoColo ? "tc" : "s");
  }
};

template <bool isTwoColo> struct Param<Clique<isTwoColo>> {
  int Nmax;
  int n, m;
  int timeout = 30;
  Param(int _Nmax, int tm = 30) : Nmax(_Nmax), n(1), m(1), timeout(tm) {}

  void init() {
    n = 1;
    m = 1;
  }

  Param operator++() {
    if (m < n * (n - 1) / 2) {
      m++;
    } else {
      n++;
      m = 1;
    }
    return *this;
  }

  int operator[](int i) {
    if (i == 0) {
      return n;
    }
    if (i == 1) {
      return m;
    }
    return -1;
  }

  bool is_end() { return n > Nmax; }

  int nb_param() { return 2; }

  Clique<isTwoColo> gen() { return Clique<isTwoColo>(n, m); }
};
#endif
