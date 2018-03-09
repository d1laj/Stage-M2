#ifndef FILL_CSV_HPP
#define FILL_CSV_HPP

#include "clique.hpp"
#include "cycle.hpp"
#include "path.hpp"
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// template <class G> struct CSV;
// template <class G> void call_compute_csv(CSV<G> *csv) { *csv->compute(); }

template <class G> struct CSV {
  Param<G> p;
  bool to_save = true;
  std::string filename;
  int nb_values = 0;
  int max_timeout = 500;
  int nb_thread = 4;
  std::mutex mut_q;
  std::mutex mut_s;
  std::vector<TernaryBoolean> values;
  std::queue<std::pair<int, Param<G>>> left;

  CSV(Param<G> _p, std::string _filename, int mt = 500, int _nb = 4)
      : p(_p), filename(_filename), max_timeout(mt), nb_thread(_nb) {}

  TernaryBoolean test(Param<G> q) {
    G graph = q.gen();
    return graph.test(q.timeout);
  }

  void init() {
    p.init();
    while (!p.is_end()) {
      values.push_back(TernaryBoolean::INDET);
      left.push(std::pair<int, Param<G>>(nb_values, p));
      ++p;
      nb_values++;
    }
    if (p.nb_param() == 2) {
      std::ifstream file(filename);
      if (file.is_open()) {
        Param<G> q = p;
        q.init();
        int ind = 0;
        while (!q.is_end() && !file.eof()) {
          char c;
          char virgule;
          file >> c >> virgule;
          if (c == '1') {
            values[ind] = TernaryBoolean::VRAI;
          } else if (c == '0') {
            values[ind] = TernaryBoolean::FAUX;
          } else if (c == '?') {
            values[ind] = TernaryBoolean::INDET;
          } else {
            std::cout << "bug : " << c << '\n';
          }
          ++q;
          ind++;
        }
        file.close();
      }
    } else {
      std::ifstream file(filename);
      if (file.is_open()) {
        Param<G> q = p;
        q.init();
        int ind = 0;
        char c;
        for (int i = 0; i <= p.nb_param(); i++) {
          file >> c;
          // std::cout << c;
          file >> c;
          // std::cout << c;
        }

        while (!q.is_end() && !file.eof()) {
          int c1;
          for (int i = 0; i <= p.nb_param(); i++) {
            file >> c1 >> c;
            // std::cout << c1 << c << " | ";
          }
          char c;
          char virgule;
          file >> c >> virgule;
          if (c == '1') {
            values[ind] = TernaryBoolean::VRAI;
          } else if (c == '0') {
            values[ind] = TernaryBoolean::FAUX;
          } else if (c == '?') {
            values[ind] = TernaryBoolean::INDET;
          } else {
            std::cout << "bug : " << c << "|" << virgule << '\n';
          }
          ++q;
          ind++;
        }
        file.close();
      }
    }
    save();
  }

  void save() {
    if (p.nb_param() == 2) {
      Param<G> q = p;
      q.init();
      int index = 0;
      std::fstream file;
      file.open(filename);
      while (!q.is_end()) {
        // std::cout << "x" << std::endl;
        if (q[1] == 1 && q[0] > 1) {
          file << '\n';
        }
        // std::cout << "y" << std::endl;
        switch (values[index]) {
        case TernaryBoolean::VRAI:
          file << " 1,";
          break;
        case TernaryBoolean::FAUX:
          file << " 0,";
          break;
        case TernaryBoolean::INDET:
          file << " ?,";
          break;
        }
        // std::cout << "z" << std::endl;
        index++;
        ++q;
      }
      file.close();
    } else {

      Param<G> q = p;
      q.init();
      int index = 0;
      std::ofstream file;
      file.open(filename);

      file << "N, K, ";
      for (int i = 0; i <= p.nb_param() - 2; i++) {
        file << (char)('a' + i) << ", ";
      }
      file << "\n";

      while (!q.is_end()) {
        // std::cout << "x" << std::endl;
        int l = 0;
        for (int i = 2; i < q.nb_param(); i++) {
          l += q[i];
        }
        file << q[0] << ", " << q[1] << ", " << q[0] - 1 - l << ", ";
        for (int i = 2; i < q.nb_param(); i++) {
          file << q[i] << ", ";
        }
        // std::cout << "y" << std::endl;
        switch (values[index]) {
        case TernaryBoolean::VRAI:
          file << " 1,";
          break;
        case TernaryBoolean::FAUX:
          file << " 0,";
          break;
        case TernaryBoolean::INDET:
          file << " ?,";
          break;
        }
        file << "\n";
        // std::cout << "z" << std::endl;
        index++;
        ++q;
      }
      file.close();
    }
  }

  void compute() {
    // sleep(10);
    while (1) {
      mut_q.lock();
      if (left.empty()) {
        mut_q.unlock();
        // std::cout << "term" << std::endl;
        mut_s.lock();
        nb_thread--;
        mut_s.unlock();
        break;
      }
      std::pair<int, Param<G>> pa = left.front();
      left.pop();
      mut_q.unlock();

      if (values[pa.first] == TernaryBoolean::INDET) {
        TernaryBoolean ret = test(pa.second);
        if (ret == TernaryBoolean::INDET) {
          pa.second.timeout *= 4;
          if (pa.second.timeout <= max_timeout) {
            mut_q.lock();
            left.push(pa);
            mut_q.unlock();
          }
        }
        // std::cout << ret << " ";
        mut_s.lock();
        values[pa.first] = ret;
        to_save = true;
        mut_s.unlock();
      }
    }
  }

  void run() {
    init();

    std::vector<std::thread> T;
    mut_s.lock();
    for (int i = 0; i < nb_thread; i++) {
      T.push_back(std::thread(&CSV<G>::compute, std::ref(*this)));
    }
    mut_s.unlock();

    std::cout << "Thread lancés\n";

    while (1) {
      // std::cout << "l" << std::endl;
      mut_q.lock();
      std::cout << left.size() << "\n";
      if (false && left.size()) {
        std::cout << left.front().second.timeout << "\n";
      }
      mut_q.unlock();

      mut_s.lock();
      // std::cout << nb_thread << "\n";
      if (nb_thread == 0) {
        std::cout << "stop" << std::endl;
        save();
        mut_s.unlock();
        break;
      }
      // std::cout << "s" << std::endl;
      if (to_save) {
        // std::cout << "i" << std::endl;
        save();
        // std::cout << "a" << std::endl;
      }
      // std::cout << "e" << std::endl;
      mut_s.unlock();
      sleep(1);
    }

    for (auto &th : T) {
      th.join();
    }
  }
};

#endif
