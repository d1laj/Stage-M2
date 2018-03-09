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
#include "param.hpp"

template <bool isTwoColo, bool edges = false> struct Graph {
  const int N;
  VariableOffsets<isTwoColo, edges> vo;
  const int K;
  std::vector<int> ed;

  Graph(int _N, int _K) : N(_N), vo(_N), K(_K) {
    ed = std::vector<int>(N * (N - 1) / 2, 0);
  }

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
        file << -vo.index(VariableType::Edge, vp)
             << -vo.index(VariableType::V, vp.u, i)
             << -vo.index(VariableType::V, vp.v, i) << End;
      }
    }
  }

  void generate_cc_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of complete coloration on K colors\n";

    file << "c  K -> color k <= K is used and k > K not used\n";

    for (int k = 0; k < N; k++) {
      if (k < K) {
        file << vo.index(VariableType::Color, k) << End;
      } else {
        file << -vo.index(VariableType::Color, k) << End;
      }
    }

    file << "c  colo is complete\n";
    file << vo.index(VariableType::Complete) << End;
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

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      file << vo.index(VariableType::Hom, cp)
           << vo.index(VariableType::HomSign, cp) << End;
    }

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
        file << vo.index(VariableType::Y, vp, cp, true);
      }
      file << End;

      file << -vo.index(VariableType::Hom, cp)
           << vo.index(VariableType::HomSign, cp);
      for (VertexPair vp(N); !vp.is_end(); vp++) {
        file << vo.index(VariableType::Y, vp, cp, false);
      }
      file << End;
    }

    for (ColorPair cp(N); !cp.is_end(); cp++) {
      if (cp.i >= K || cp.j >= K) {
        file << -vo.index(VariableType::Hom, cp) << End;
      }
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

    if (isTwoColo) {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        for (int k = 0; k < N; k++) {
          if (k != cp.i && k != cp.j) {
            file << -vo.index(VariableType::X, cp, k)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.i, k)) << End;
            file << -vo.index(VariableType::X, cp, k)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;

            file << -vo.index(VariableType::X, cp, k)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;
            file << -vo.index(VariableType::X, cp, k)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;

            file << vo.index(VariableType::X, cp, k)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
            file << vo.index(VariableType::X, cp, k)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
          }
        }
      }
    } else {
      for (ColorPair cp(N); !cp.is_end(); cp++) {
        for (int k = 0; k < N; k++) {
          if (k != cp.i && k != cp.j) {
            file << -vo.index(VariableType::X, cp, k, true)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.i, k)) << End;
            file << -vo.index(VariableType::X, cp, k, true)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;

            file << -vo.index(VariableType::X, cp, k, true)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;
            file << -vo.index(VariableType::X, cp, k, true)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;

            file << vo.index(VariableType::X, cp, k, true)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
            file << vo.index(VariableType::X, cp, k, true)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
          }
        }
      }

      for (ColorPair cp(N); !cp.is_end(); cp++) {
        for (int k = 0; k < N; k++) {
          if (k != cp.i && k != cp.j) {
            file << -vo.index(VariableType::X, cp, k, false)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.i, k)) << End;
            file << -vo.index(VariableType::X, cp, k, false)
                 << vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;

            file << -vo.index(VariableType::X, cp, k, false)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;
            file << -vo.index(VariableType::X, cp, k, false)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << End;

            file << vo.index(VariableType::X, cp, k, false)
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
            file << vo.index(VariableType::X, cp, k, false)
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.i, k))
                 << vo.index(VariableType::HomSign, ColorPair(N, cp.j, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.i, k))
                 << -vo.index(VariableType::Hom, ColorPair(N, cp.j, k)) << End;
          }
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
    for (ColorPair cp(K, N); !cp.is_end(); cp++) {
      file << -vo.index(VariableType::Complete)
           << vo.index(VariableType::Comp, cp) << End;
    }

    file << vo.index(VariableType::Complete);
    for (ColorPair cp(K, N); !cp.is_end(); cp++) {
      file << -vo.index(VariableType::Comp, cp);
    }
    file << End;
  }

  void generate_Comp_definition(std::ofstream &file) {
    file << "c ---------------------------\n";
    file << "c definition of Comp\n";

    for (ColorPair cp(K, N); !cp.is_end(); cp++) {
      file << -vo.index(VariableType::Hom, cp)
           << vo.index(VariableType::Comp, cp) << End;
      // file << vo.index(VariableType::Hom, cp)
      //     << -vo.index(VariableType::Comp, cp, 0) << End;
    }

    for (ColorPair cp(K, N); !cp.is_end(); cp++) {
      if (isTwoColo) {
        for (int k = 0; k < K; k++) {
          if (k != cp.i && k != cp.j) {
            file << -vo.index(VariableType::X, cp, k)
                 << vo.index(VariableType::Comp, cp) << End;
          }
        }

        file << -vo.index(VariableType::Comp, cp)
             << vo.index(VariableType::Hom, cp);
        for (int k = 0; k < K; k++) {
          if (k != cp.i && k != cp.j) {
            file << vo.index(VariableType::X, cp, k);
          }
        }
        file << End;
      } else {
        for (int k = 0; k < K; k++) {
          for (int l = 0; l < K; l++) {
            if (k != l && k != cp.i && l != cp.i && k != cp.j && l != cp.j) {
              file << -vo.index(VariableType::X, cp, k, true)
                   << -vo.index(VariableType::X, cp, l, false)
                   << vo.index(VariableType::Comp, cp) << End;
            }
          }
        }

        file << -vo.index(VariableType::Comp, cp)
             << vo.index(VariableType::Hom, cp);
        for (int k = 0; k < K; k++) {
          if (k != cp.i && k != cp.j) {
            file << vo.index(VariableType::X, cp, k, true);
          }
        }
        file << End;

        file << -vo.index(VariableType::Comp, cp)
             << vo.index(VariableType::Hom, cp);
        for (int k = 0; k < K; k++) {
          if (k != cp.i && k != cp.j) {
            file << vo.index(VariableType::X, cp, k, false);
          }
        }
        file << End;
      }
    }
  }

  void generate_help(std::ofstream &file) {
    for (int i = 0; i < N; i++) {
      for (int k = i + 1; k < N; k++) {
        file << -vo.index(VariableType::V, i, k) << End;
      }
    }
  }

  void gen_edges(std::ofstream &file) {
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      if (ed[vp.index()] == 1 || ed[vp.index()] == 2 || ed[vp.index()] == 3) {
        file << vo.index(VariableType::Edge, vp) << End;
        if (ed[vp.index()] == 2) {
          file << vo.index(VariableType::EdgeSign, vp) << End;
        } else if (ed[vp.index()] == 3) {
          file << -vo.index(VariableType::EdgeSign, vp) << End;
        }
      } else if (ed[vp.index()] == 4) {
        file << -vo.index(VariableType::Edge, vp) << End;
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
    generate_help(file);
    gen_edges(file);
  }

  virtual void generate(std::string filename) {
    std::ofstream file;
    file.open(filename);
    generate_graph_clauses(file);
    file.close();
  }

  TernaryBoolean minisat(std::string filename, std::string outfile, int timeout,
                         bool output = true) {
    int pid = fork();
    int fd;
    if (pid == 0) {
      if (!output) {
        fd = open("/dev/null", O_WRONLY | O_CREAT,
                  0666); // open the file /dev/null
        dup2(fd, 1);     // replace standard output with output file
        dup2(fd, 2);
      }
      execl("./glucose_static",
            "glucose_static", /*"-rnd-init", "-rnd-freq=0.1",*/
            filename.c_str(), outfile.c_str(),
            (char *)0); // Excecute the command
      if (!output) {
        close(fd);
      }
    }
    int status;
    int ret = waitpid(pid, &status, WNOHANG);
    int k = 0;
    while (!ret && (k < 10 * timeout || timeout == 0)) {
      k++;
      usleep(100000);
      ret = waitpid(pid, &status, WNOHANG);
    }
    if (!ret) {
      // std::cout << k << " " << std::endl;
      kill(pid, SIGINT);
      wait(0);
      return TernaryBoolean::INDET;
    }
    wait(0);

    std::ifstream file;
    file.open(outfile);
    std::string firstword;
    file >> firstword;
    file.close();

    if (firstword == "UNSAT") {
      return TernaryBoolean::FAUX;
    }
    return TernaryBoolean::VRAI;
  }

  void to_graph(std::string filename, std::string outfile) {
    std::ifstream ifile;
    ifile.open(filename);

    std::ofstream ofile;
    ofile.open(outfile);
    ofile << "graph {" << '\n';
    int val;

    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        ifile >> val;
        if (val > 0) {
          ofile << "a" + std::to_string(i) << "[label=" + std::to_string(j)
                << "];\n";
        }
      }
    }

    std::vector<std::pair<int, int>> m;
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      m.push_back(std::pair<int, int>(0, 0));
      int i;
      ifile >> i;
      m[vp.index()].first = i;
    }
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      int i;
      ifile >> i;
      m[vp.index()].second = i;
    }

    for (VertexPair vp(N); !vp.is_end(); vp++) {
      if (m[vp.index()].first > 0) {
        if (m[vp.index()].second > 0) {
          ofile << "a" + std::to_string(vp.u) + " -- " + "a" +
                       std::to_string((vp.v))
                << ";\n";
        } else {
          ofile << "a" + std::to_string(vp.u) + " -- " + "a" +
                       std::to_string((vp.v))
                << " [style=dotted];\n";
        }
      }
    }

    m.clear();
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      m.push_back(std::pair<int, int>(0, 0));
      int i;
      ifile >> i;
      m[vp.index()].first = i;
    }
    for (VertexPair vp(N); !vp.is_end(); vp++) {
      int i;
      ifile >> i;
      m[vp.index()].second = i;
    }

    for (VertexPair vp(N); !vp.is_end(); vp++) {
      if (m[vp.index()].first > 0) {
        if (m[vp.index()].second > 0) {
          ofile << "c" + std::to_string(vp.u) + " -- " + "c" +
                       std::to_string((vp.v))
                << ";\n";
        } else {
          ofile << "c" + std::to_string(vp.u) + " -- " + "c" +
                       std::to_string((vp.v))
                << " [style=dotted];\n";
        }
      }
    }

    ofile << "}\n";
    ofile.close();
    ifile.close();
  }

  void pdf(std::string filename, std::string outfile) {
    int pid = fork();
    if (pid == 0) {
      execl("/usr/bin/dot", "dot", "-Tpdf", filename.c_str(), "-o",
            outfile.c_str(),
            (char *)0); // Excecute the command
    }
    wait(0);
  }

  virtual std::string name() {
    return "graph_" + std::to_string(N) + "_" + std::to_string(K) + "_" +
           (isTwoColo ? "tc" : "s");
  }

  void clean(std::string str) { remove(str.c_str()); }

  TernaryBoolean test(int timeout = 0, bool output = false,
                      bool cleanup = true) {
    std::string filename = name();
    generate("result/cnf/" + filename + ".cnf");

    TernaryBoolean b =
        minisat("result/cnf/" + filename + ".cnf",
                "result/sol/" + filename + ".sol", timeout, output);

    if (b == TernaryBoolean::VRAI) {
      to_graph("result/sol/" + filename + ".sol",
               "result/dot/" + filename + ".dot");
      pdf("result/dot/" + filename + ".dot", "result/pdf/" + filename + ".pdf");
    }
    if (cleanup) {
      clean("result/cnf/" + filename + ".cnf");
      clean("result/sol/" + filename + ".sol");
    }
    return b;
  }
};

template <bool isTwoColo> struct Param<Graph<isTwoColo>> {
  int Nmax;
  int n, k;
  int timeout = 30;
  Param(int _Nmax) : Nmax(_Nmax), n(1), k(1) {}

  void init() {
    n = 1;
    k = 1;
  }

  Param operator++() {
    if (k < n) {
      k++;
    } else {
      n++;
      k = 1;
    }
    return *this;
  }

  int operator[](int i) {
    if (i == 0) {
      return n;
    }
    if (i == 1) {
      return k;
    }
    return -1;
  }

  bool is_end() { return n > Nmax; }

  int nb_param() { return 2; }

  Graph<isTwoColo> gen() { return Graph<isTwoColo>(n, k); }
};
#endif
