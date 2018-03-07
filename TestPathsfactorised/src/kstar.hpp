#ifndef KSTARPATH_HPP
#define KSTARPATH_HPP

#include "graph.hpp"

template <bool isTwoColo, int k> struct KStarPath : public Graph<isTwoColo> {
  std::array<int, k - 1> legs;

  KStarPath(int _N, int _K) : Graph<isTwoColo>(_N, _K) {}

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
    std::string str = std::to_string(k) + "starpath_" +
                      std::to_string(this->N) + "_" + std::to_string(this->K) +
                      "_" + (isTwoColo ? "tc" : "s");
    int t = 0;
    for (int i = k - 1; i >= 0; i--) {
      t += legs[i];
      str += std::to_string(legs[i]) + "_";
    }
    str += std::to_string(this->N - t);
    return str;
  }
};

template <bool isTwoColo, int k> struct Param<KStarPath<isTwoColo, k>> {
  int Nmax;
  int n, p;
  std::array<int, k - 1> legs;
  int timeout = 30;

  Param(int _Nmax) : Nmax(_Nmax), n(1), p(1) {}

  void init() {
    n = 1;
    p = 1;
    for (int i = 0; i < k; i++) {
      legs[i] = 0;
    }
  }

  Param operator++() {
    for (int i = k + 1; i >= 0; i--) {
      (*this)[i]++;
      if (i > 0 && (*this)[i] > n) {
        (*this)[i] = 0;
      } else {
        break;
      }
    }

    int l = 0;
    for (int i = k + 1; i >= 3; i--) {
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

    if (l > n || (*this)[2] > n - (*this)[2]) {
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

  KStarPath<isTwoColo, k> gen() { return KStarPath<isTwoColo, k>(n, p); }
};
#endif
