#ifndef KSTARPATH_HPP
#define KSTARPATH_HPP

#include "graph.hpp"

template <bool isTwoColo, int k> struct KStarPath : public Graph<isTwoColo> {
  std::array<int, k - 1> legs;

  KStarPath(int _N, int _K, std::array<int, k - 1> _legs)
      : Graph<isTwoColo>(_N, _K), legs(_legs) {}

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    this->generate_graph_clauses(file);

    file << "c--------------------\n";

    int l = 0;
    for (int i = 0; i < k - 1; i++) {
      l += legs[i];
    }
    l = this->N - 1 - l;

    int offset = l;
    for (int i = 1; i <= l; i++) {
      file << this->vo.index(VariableType::Edge, VertexPair(this->N, i - 1, i))
           << End;
      for (int j = 0; j < this->N; j++) {
        if (j != i && j != i - 1 && (j != i + 1 || i == l)) {
          file << -this->vo.index(VariableType::Edge, VertexPair(this->N, i, j))
               << End;
        }
      }
    }

    for (int p = 0; p < k - 1; p++) {
      for (int i = 1; i <= legs[p]; i++) {
        file << this->vo.index(VariableType::Edge,
                               VertexPair(this->N, i - 1 + (i - 1 ? offset : 0),
                                          i + offset))
             << End;
        for (int j = 0; j < this->N; j++) {
          if (j != i + offset && j != i - 1 + (i - 1 ? offset : 0) &&
              (j != i + offset + 1 || i == legs[p])) {
            file << -this->vo.index(VariableType::Edge,
                                    VertexPair(this->N, i + offset, j))
                 << End;
          }
        }
      }
      offset += legs[p];
    }

    file.close();
  }

  virtual std::string name() {
    std::string str = std::to_string(k) + "starpath_" +
                      std::to_string(this->N) + "_" + std::to_string(this->K) +
                      "_" + (isTwoColo ? "tc" : "s") + "_";
    int t = 0;
    for (int i = k - 2; i >= 0; i--) {
      t += legs[i];
      str += std::to_string(legs[i]) + "_";
    }
    str += std::to_string(this->N - t - 1);
    return str;
  }
};

template <bool isTwoColo, int k> struct Param<KStarPath<isTwoColo, k>> {
  int Nmax;
  int n, p;
  std::array<int, k - 1> legs;
  int timeout = 30;

  Param(int _Nmax, int tm = 30) : Nmax(_Nmax), n(1), p(1), timeout(tm) {}

  void init() {
    n = 1;
    p = 1;
    for (int i = 0; i < k; i++) {
      legs[i] = 0;
    }
  }

  Param operator++() {
    for (int i = k; i >= 0; i--) {
      (*this)[i]++;
      if (i > 0 && (*this)[i] > n) {
        (*this)[i] = 0;
      } else {
        break;
      }
    }

    int l = 0;
    for (int i = k; i >= 3; i--) {
      if ((*this)[i - 1] < (*this)[i]) {
        for (int j = i; j < k + 2; j++) {
          (*this)[j] = 0;
        }
        (*this)[i - 1]++;
      }
    }

    for (int j = 2; j < k + 2; j++) {
      l += (*this)[j];
    }
    l += (*this)[2];

    if (l >= n || (*this)[2] > n - 1 - (*this)[2]) {
      for (int j = 2; j < k + 2; j++) {
        (*this)[j] = 0;
      }
      p++;
      if (p > n) {
        p = 1;
        n++;
      }
    }

    return *this;
  }

  int &operator[](int i) {
    if (i == 0) {
      return n;
    }
    if (i == 1) {
      return p;
    }
    return legs[i - 2];
  }

  bool is_end() { return n > Nmax; }

  int nb_param() { return k + 1; }

  KStarPath<isTwoColo, k> gen() { return KStarPath<isTwoColo, k>(n, p, legs); }
};
#endif
