#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<int> lit;
vector<vector<int>> clause_list;

void see_lit() {
  for (auto i : lit) {
    cout << i << " ";
  }
  cout << '\n';
}

void see_clause() {

  for (auto &c : clause_list) {
    bool f = true;
    for (auto i : c) {
      if (f) {
        cout << i << ": ";
      } else {
        cout << i << " ";
      }
      f = false;
    }
    cout << "0\n";
  }
  cout << '\n';
}

bool has_uni(int &v) {
  for (auto &c : clause_list) {
    if (c.size() == 2) {
      v = c[1];
      return true;
    }
  }
  return false;
}

bool has_empty_clause() {
  for (auto &c : clause_list) {
    if (c.size() == 1) {
      return true;
    }
  }
  return false;
}

void remove(int v) {
  unsigned int i = 0;
  while (i < clause_list.size()) {
    auto &c = clause_list[i];
    unsigned int j = 1;
    while (j < c.size()) {
      if (c[j] == -v) {
        c.erase(c.begin() + j);
      } else if (c[j] == v) {
        clause_list.erase(clause_list.begin() + i);
        i--;
        break;
      } else {
        j++;
      }
    }
    i++;
  }
}

int abso(int a) { return (a < 0 ? -a : a); }

int main() {
  clause_list.push_back(vector<int>(1, 1));

  std::ifstream file;
  file.open("test.cnf");

  int current = 0;
  int maximum = 0;
  int a;
  while (file >> a) {
    maximum = (a > maximum ? a : maximum);
    if (a == 0) {
      current++;
      clause_list.push_back(vector<int>(1, current + 1));
    } else {
      clause_list[current].push_back(a);
    }
  }
  clause_list.erase(clause_list.end());
  lit = vector<int>(maximum + 1, 0);
  file.close();

  see_clause();
  see_lit();

  while (clause_list.size() || !has_empty_clause()) {
    int v;
    if (has_uni(v)) {
      cout << "ded " << v << "\n";
      lit[abso(v)] = v;
      remove(v);
      continue;
    }
    see_clause();
    see_lit();

    cin >> v;
    lit[abso(v)] = v;
    remove(v);
  }

  if (has_empty_clause()) {
    cout << "boom\n";
  }
}
