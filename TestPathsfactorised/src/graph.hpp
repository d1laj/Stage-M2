#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <array>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Variable.hpp"

struct Graph {
  const int N;
  VariableOffsets vo;
  Graph(int _N) : N(_N), vo(_N) {}

  void generate_colo_rules(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c each vertex has one color\n";
    for (int u = 0; u < N; u++) {
      for (int i = 0; i < N; i++) {
        file << vo.index(VariableType::V, u, i);
      }
      file << End;
    }
    file << "c each vertex doesn't have two color\n";
    for (int u = 0; u < N; u++) {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        file << -vo.index(VariableType::V, u, cp.i)
             << -vo.index(VariableType::V, u, cp.j) << End;
      }
    }

    file << "c each edge has 2 colors\n";
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      for (int i = 0; i < N; i++) {
        file << -vo.index(VariableType::V, vp.u, i)
             << -vo.index(VariableType::V, vp.v, i) << End;
      }
    }
  }

  void generate_cc_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variable CC_i\n";

    file << "c  CC_i -> color k <= i is used and k > i not used\n";
    for (int i = 0; i < N; i++) {
      for (int k = 0; k < N; k++) {
        if (k <= i) {
          file << -vo.index(VariableType::CC, i)
               << vo.index(VariableType::Color, k) << End;
        } else {
          file << -vo.index(VariableType::CC, i)
               << -vo.index(VariableType::Color, k) << End;
        }
      }
    }

    file << "c  CC_i -> colo is complete\n";
    for (int i = 0; i < N; i++) {
      file << -vo.index(VariableType::CC, i)
           << vo.index(VariableType::Complete, i) << End;
    }

    file << "c  CC_i <- colo is complete and has only first i color\n";
    for (int i = 0; i < N; i++) {
      file << vo.index(VariableType::CC, i)
           << -vo.index(VariableType::Complete, i);
      for (int k = 0; k < N; k++) {
        if (k <= i) {
          file << -vo.index(VariableType::Color, k);
        } else {
          file << vo.index(VariableType::Color, k);
        }
      }
      file << End;
    }
  }

  void generate_edgesign_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variable EdgeSign\n";

    for (VertexPair vp(N); !vp.is_end(); vp++) {
      file << vo.index(VariableType::Edge, vp)
           << vo.index(VariableType::EdgeSign, vp) << End;
    }
  }

  void generate_hom_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variables Hom and HomSign\n";

    for (VertexPair vp(N); !vp.is_end(); vp++) {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        file << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::V, vp.u, cp.i)
             << -vo.index(VariableType::V, vp.v, cp.j)
             << vo.index(VariableType::Hom, cp) << End;
        file << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.i)
             << -vo.index(VariableType::V, vp.v, cp.j)
             << vo.index(VariableType::HomSign, cp) << End;
        file << -vo.index(VariableType::Edge, vp)
             << vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.i)
             << -vo.index(VariableType::V, vp.v, cp.j)
             << -vo.index(VariableType::HomSign, cp) << End;

        file << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::V, vp.u, cp.j)
             << -vo.index(VariableType::V, vp.v, cp.i)
             << vo.index(VariableType::Hom, cp) << End;
        file << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.j)
             << -vo.index(VariableType::V, vp.v, cp.i)
             << vo.index(VariableType::HomSign, cp) << End;
        file << -vo.index(VariableType::Edge, vp)
             << vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.j)
             << -vo.index(VariableType::V, vp.v, cp.i)
             << -vo.index(VariableType::HomSign, cp) << End;
      }
    }

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      file << -vo.index(VariableType::Hom, cp)
           << -vo.index(VariableType::HomSign, cp);
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        file << -vo.index(VariableType::Y, vp, cp, true);
      }
      file << End;

      file << -vo.index(VariableType::Hom, cp)
           << vo.index(VariableType::HomSign, cp);
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        file << -vo.index(VariableType::Y, vp, cp, false);
      }
      file << End;
    }
  }

  void generate_color_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variable color\n";

    for (int i = 0; i < N; i++) {
      file << -vo.index(VariableType::Color, i);
      for (int u = 0; u < N; u++) {
        file << vo.index(VariableType::V, u, i);
      }
      file << End;

      for (int u = 0; u < N; u++) {
        file << -vo.index(VariableType::V, u, i)
             << vo.index(VariableType::Color, i) << End;
      }
    }
  }

  void generate_X_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variable X\n";

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      for (int k = 0; k < N; k++) {
        if (k != cp.i && k != cp.j) {
          file << -vo.index(VariableType::X, cp, k)
               << -vo.index(VariableType::Hom, ColorPair(cp.i, k)) << End;
          file << -vo.index(VariableType::X, cp, k)
               << -vo.index(VariableType::Hom, ColorPair(cp.j, k)) << End;

          file << -vo.index(VariableType::X, cp, k)
               << -vo.index(VariableType::HomSign, ColorPair(cp.i, k))
               << -vo.index(VariableType::HomSign, ColorPair(cp.j, k)) << End;
          file << -vo.index(VariableType::X, cp, k)
               << vo.index(VariableType::HomSign, ColorPair(cp.i, k))
               << vo.index(VariableType::HomSign, ColorPair(cp.j, k)) << End;

          file << vo.index(VariableType::X, cp, k)
               << -vo.index(VariableType::HomSign, ColorPair(cp.i, k))
               << vo.index(VariableType::HomSign, ColorPair(cp.j, k)) << End;
          file << vo.index(VariableType::X, cp, k)
               << vo.index(VariableType::HomSign, ColorPair(cp.i, k))
               << -vo.index(VariableType::HomSign, ColorPair(cp.j, k)) << End;
        }
      }
    }
  }

  void generate_Y_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of variable Y\n";

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        file << -vo.index(VariableType::Y, vp, cp, false)
             << vo.index(VariableType::Edge, vp) << End;
        file << -vo.index(VariableType::Y, vp, cp, false)
             << -vo.index(VariableType::EdgeSign, vp) << End;
        file << -vo.index(VariableType::Y, vp, cp, false)
             << vo.index(VariableType::V, vp.u, cp.i)
             << vo.index(VariableType::V, vp.v, cp.i) << End;
        file << -vo.index(VariableType::Y, vp, cp, false)
             << vo.index(VariableType::V, vp.u, cp.j)
             << vo.index(VariableType::V, vp.v, cp.j) << End;

        file << -vo.index(VariableType::Y, vp, cp, true)
             << vo.index(VariableType::Edge, vp) << End;
        file << -vo.index(VariableType::Y, vp, cp, true)
             << vo.index(VariableType::EdgeSign, vp) << End;
        file << -vo.index(VariableType::Y, vp, cp, true)
             << vo.index(VariableType::V, vp.u, cp.i)
             << vo.index(VariableType::V, vp.v, cp.i) << End;
        file << -vo.index(VariableType::Y, vp, cp, true)
             << vo.index(VariableType::V, vp.u, cp.j)
             << vo.index(VariableType::V, vp.v, cp.j) << End;

        file << vo.index(VariableType::Y, vp, cp, false)
             << -vo.index(VariableType::Edge, vp)
             << vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.i)
             << -vo.index(VariableType::V, vp.v, cp.j) << End;
        file << vo.index(VariableType::Y, vp, cp, false)
             << -vo.index(VariableType::Edge, vp)
             << vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.j)
             << -vo.index(VariableType::V, vp.v, cp.i) << End;

        file << vo.index(VariableType::Y, vp, cp, true)
             << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.i)
             << -vo.index(VariableType::V, vp.v, cp.j) << End;
        file << vo.index(VariableType::Y, vp, cp, true)
             << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::EdgeSign, vp)
             << -vo.index(VariableType::V, vp.u, cp.j)
             << -vo.index(VariableType::V, vp.v, cp.i) << End;
      }
    }
  }
  void generate_complete_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of Complete\n";
    for (int k = 0; k < N; k++) {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        file << -vo.index(VariableType::Complete, k)
             << vo.index(VariableType::Comp, cp, k) << End;
      }

      file << vo.index(VariableType::Complete, k);
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        file << -vo.index(VariableType::Comp, cp, k);
      }
      file << End;
    }
  }

  void generate_Comp_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of Comp\n";

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      file << -vo.index(VariableType::Hom, cp)
           << vo.index(VariableType::Comp, cp, 0) << End;
      file << vo.index(VariableType::Hom, cp)
           << -vo.index(VariableType::Comp, cp, 0) << End;
    }

    for (int k = 1; k < N; k++) {
      for (ColorPair cp(k + 1); !cp.is_end(); cp++) {
        file << -vo.index(VariableType::Comp, cp, k - 1)
             << vo.index(VariableType::Comp, cp, k) << End;
        if (cp.i == k || cp.j == k) {
          for (int l = 0; l < k; l++) {
            if (l != cp.i && l != cp.j) {
              file << -vo.index(VariableType::X, cp, l)
                   << vo.index(VariableType::Comp, cp, k) << End;
            }
          }
        } else {
          file << -vo.index(VariableType::X, cp, k)
               << vo.index(VariableType::Comp, cp, k) << End;
        }
      }

      for (ColorPair cp(k + 1); !cp.is_end(); cp++) {
        if (cp.i == k || cp.j == k) {
          file << vo.index(VariableType::Comp, cp, k - 1);
          for (int l = 0; l < k; l++) {
            if (l != cp.i && l != cp.j) {
              file << vo.index(VariableType::X, cp, l);
            }
          }
          file << -vo.index(VariableType::Comp, cp, k) << End;
        } else {
          file << vo.index(VariableType::X, cp, k)
               << vo.index(VariableType::Comp, cp, k - 1)
               << -vo.index(VariableType::Comp, cp, k) << End;
        }
      }

      for (ColorPair cp(N); !cp.is_end(); cp++) {
        if (cp.i > k || cp.j > k) {
          file << vo.index(VariableType::Comp, cp, k)
               << -vo.index(VariableType::Comp, cp, k - 1) << End;
          file << -vo.index(VariableType::Comp, cp, k)
               << vo.index(VariableType::Comp, cp, k - 1) << End;
        }
      }
    }
  }

  void generate_graph_clauses(std::ofstream &file) {
    file << "p cnf 0 0\n";
    generate_cc_definition(file);
    generate_colo_rules(file);
    generate_color_definition(file);
    generate_Comp_definition(file);
    generate_complete_definition(file);
    generate_edgesign_definition(file);
    generate_hom_definition(file);
    generate_X_definition(file);
    generate_Y_definition(file);
  }

  void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    generate_graph_clauses(file);
    file.close();
  }

  bool minisat(std::string filename, std::string outfile, int timeout) {
    int pid = fork();
    if (pid == 0) {
      execl("./glucose_static", "glucose_static", "-rnd-init", "-rnd-freq=0.1",
            filename.c_str(), outfile.c_str(), (char *)0);
    }
    int status;
    int ret = waitpid(pid, &status, WNOHANG);
    int k = 0;
    while (!ret && (k < timeout || timeout == 0)) {
      k++;
      sleep(1);
      ret = waitpid(pid, &status, WNOHANG);
    }
    if (!ret) {
      kill(pid, SIGINT);
      wait(0);
      return false;
    }
    wait(0);

    std::ifstream file;
    file.open(outfile);
    std::string firstword;
    file >> firstword;
    file.close();

    if (firstword == "UNSAT") {
      return false;
    }
    return true;
  }
  /*
    void to_graph(std::string filename, std::string outfile) {
      std::ifstream ifile;
      ifile.open(filename);

      std::ofstream ofile;
      ofile.open(outfile);
      ofile << "graph {" << '\n';

      int val;
      for (int i = 0; i < n; i++) {
        ifile >> val;
        if (val > 0) {
          ofile << "a" + std::to_string(i) + " -- " + "a" +
                       std::to_string((i + 1) % n)
                << ";\n";
        } else {
          ofile << "a" + std::to_string(i) + " -- " + "a" +
                       std::to_string((i + 1) % n)
                << " [style=dotted];\n";
        }
      }

      for (int i = 0; i < n; i++) {
        for (int j = 0; j < K; j++) {
          ifile >> val;
          if (val > 0) {
            ofile << "a" + std::to_string(i) << "[label=" + std::to_string(j)
                  << "];\n";
          }
        }
      }

      ofile << "}\n";
      ofile.close();
      ifile.close();
    }*/
};
#endif
