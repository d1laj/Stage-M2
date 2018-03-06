#ifndef PATH_HPP
#define CATH_HPP

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
};
#endif
