#ifndef CLIQUE_HPP
#define CLIQUE_HPP

#include "graph.hpp"

template <bool isTwoColo> struct Clique : public Graph<isTwoColo> {
  int M;
  Clique(int _N, int _M) : Graph<isTwoColo, true>(_N, _N), M(_M) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    this->generate_graph_clauses(file);
    generate_nb_edges_rules(file);

    file << this->vo.index(VariableType::NbEdges,
                           VertexPair(this->N, this->N - 2, this->N - 1), M);
    file.close();
  }

  void generate_nb_edges_rules(std::ofstream &file) {
    for (VertexPair vp(this->N); !vp.is_end(); vp++) {
      if (vp.index() == 0) {
        file << -this->vo.index(VariableType::NbEdges, vp.index(), 0)
             << -this->vo.index(VariableType::Edge, vp.index()) << End;
        file << this->vo.index(VariableType::NbEdges, vp.index(), 0)
             << this->vo.index(VariableType::Edge, vp.index()) << End;

        for (int i = 1; i < M; i++) {
          file << this->vo.index(VariableType::NbEdges, vp.index(), i) << End;
        }
      }
      for (int i = 0; i < M; i++) {
        if (i > 0) {
          file << -this->vo.index(VariableType::NbEdges, vp.index(), i - 1)
               << this->vo.index(VariableType::NbEdges, vp.index(), i) << End;
        }
        file << -this->vo.index(VariableType::NbEdges, vp.index() - 1, i)
             << this->vo.index(VariableType::Edge, vp.index())
             << this->vo.index(VariableType::NbEdges, vp.index(), i) << End;

        if (i > 0) {
          file << this->vo.index(VariableType::NbEdges, vp.index(), i - 1)
               << this->vo.index(VariableType::NbEdges, vp.index() - 1, i)
               << -this->vo.index(VariableType::NbEdges, vp.index(), i) << End;

          file << this->vo.index(VariableType::NbEdges, vp.index(), i - 1)
               << -this->vo.index(VariableType::Edge, vp.index())
               << -this->vo.index(VariableType::NbEdges, vp.index(), i) << End;
        } else {
          file << this->vo.index(VariableType::NbEdges, vp.index() - 1, i)
               << -this->vo.index(VariableType::NbEdges, vp.index(), i) << End;

          file << -this->vo.index(VariableType::Edge, vp.index())
               << -this->vo.index(VariableType::NbEdges, vp.index(), i) << End;
        }
      }
    }
  }
};
#endif
