#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "graph.hpp"

template <bool isTwoColo> struct Cycle : public Graph<isTwoColo> {
  bool an = false;
  Cycle(int _N, int _K, bool _an = false) : Graph<isTwoColo>(_N, _K), an(_an) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    this->generate_graph_clauses(file);

    for (VertexPair vp(this->N); !vp.is_end(); vp++) {
      if (vp.v - vp.u != 1 && (vp.v != this->N - 1 || vp.u != 0)) {
        file << -this->vo.index(VariableType::Edge, vp) << End;
      } else {
        file << this->vo.index(VariableType::Edge, vp) << End;
        if (an) {
          if (vp.u % 2 == 0 && (vp.u != 0 || vp.v != this->N - 1)) {
            file << this->vo.index(VariableType::EdgeSign, vp) << End;
          } else {
            file << -this->vo.index(VariableType::EdgeSign, vp) << End;
          }
        }
      }
    }
    file.close();
  }
};
#endif
