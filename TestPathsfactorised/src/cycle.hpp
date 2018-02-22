#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "graph.hpp"

struct Cycle : public Graph {
  bool an = false;
  Cycle(int _N, int _K, bool _an = false) : Graph(_N), an(_an) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    generate_graph_clauses(file);

    for (VertexPair vp(N); !vp.is_end(); vp++) {
      if (vp.v - vp.u != 1 && vp.v != N - 1 && vp.u != 0) {
        file << -vo.index(VariableType::Edge, vp) << End;
      } else if (an) {
        if (vp.u % 2 == 0 && vp.v != N - 1) {
          file << vo.index(VariableType::EdgeSign, vp) << End;
        } else {
          file << -vo.index(VariableType::EdgeSign, vp) << End;
        }
      }
    }
    file.close();
  }
};
#endif
