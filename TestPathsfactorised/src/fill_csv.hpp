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
  int nb_thread = 4;
  std::mutex mut_q;
  std::mutex mut_s;
  std::vector<TernaryBoolean> values;
  std::queue<std::pair<int, Param<G>>> left;

  CSV(Param<G> _p, std::string _filename) : p(_p), filename(_filename) {}

  TernaryBoolean test(Param<G> q) {
    G graph = q.gen();
    return graph.test(q.timeout);
  }

  void init() {
    if (p.nb_param() == 2) {
      p.init();
      while (!p.is_end()) {
        values.push_back(TernaryBoolean::INDET);
        left.push(std::pair<int, Param<G>>(nb_values, p));
        ++p;
        nb_values++;
      }
    }
  }

  void save() {
    if (p.nb_param() == 2) {
      Param<G> q = p;
      q.init();
      int index = 0;
      std::ofstream file;
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

      TernaryBoolean ret = test(pa.second);
      if (ret == TernaryBoolean::INDET) {
        pa.second.timeout *= 4;
        if (pa.second.timeout <= 480) {
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
