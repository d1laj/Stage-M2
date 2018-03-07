#ifndef PARAM_HPP
#define PARAM_HPP

#include <iostream>

enum class TernaryBoolean { VRAI, FAUX, INDET };

std::ostream &operator<<(std::ostream &os, TernaryBoolean v) {
  switch (v) {
  case TernaryBoolean::VRAI:
    os << "1";
    break;
  case TernaryBoolean::FAUX:
    os << "0";
    break;
  default:
    os << "?";
  }
  return os;
}

template <class G> struct Param;

#endif
