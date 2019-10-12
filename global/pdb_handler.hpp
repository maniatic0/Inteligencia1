#pragma once
#ifndef PDB_HANDLER_HPP
#define PDB_HANDLER_HPP 1

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cassert>


class PDBHandler {
public:
  PDBHandler() = default;
  PDBHandler(PDBHandler &&) = default;
  PDBHandler(const PDBHandler &) = delete;
  PDBHandler &operator=(PDBHandler &&) = default;
  PDBHandler &operator=(const PDBHandler &) = delete;
  ~PDBHandler() = default;

  /* Load PBBs from the arguments passed to the program. Startpos is the
   * position from which start reading the PDBs names. The first argument must
   * always be 'm' (max) or 'a' (addition) to select the heuristic function */
  bool Load(int argc, const char **argv, int startPos = 1) {

    if (startPos >= argc - 1) {
      std::printf("Not enough arguments\n");
      std::printf("Arguments must be: m|a [pdb_name]+");
      return false;
    }

    if (strlen(argv[startPos]) != 1) {
      std::printf("The first argument must always be 'm' (max) or 'a' "
                  "(addition) to select the heuristic function");
      std::printf(" but it was %s\n", argv[startPos]);
      return false;
    }

    m_useMax = std::tolower(argv[startPos][0]) == 'm';

    if (!m_useMax) {
      if (std::tolower(argv[startPos][0]) != 'a') {
        std::printf("The first argument must always be 'm' (max) or 'a' "
                    "(addition) to select the heuristic function");
        std::printf(" but it was %s\n", argv[startPos]);
      }
      std::printf("Using an Addition Heuristic\n");
    } else {
      std::printf("Using an Maximum Heuristic\n");
    }

    m_pdbs.reserve(argc - startPos - 1);

    for (int i = startPos + 1; i < argc; ++i) {
      PDB pdb;

      if (!pdb.Load(argv[i])) {
        return false;
      }

      m_pdbs.push_back(std::move(pdb));
    }
    return true;
  }

  /* Get the Heuristic Value using PDBs for a state */
  long long unsigned GetValue(const state_t *state) {
    long long unsigned res = 0;
    for(size_t i=0; i<m_pdbs.size(); ++i) {
      res += m_pdbs[i].GetValue(&m_asbtractStateHolder, state);
    }
    return res;
  }

private:
  using PDB = struct PDB {
    /* Abstraction used by the pdb */
    std::unique_ptr<abstraction_t> m_abstraction;
    /* PDB heuristic */
    std::unique_ptr<state_map_t> m_map;

    /* Max File Name Size*/
    constexpr static unsigned k_maxFileNameSize = 1024;

    PDB() = default;
    PDB(PDB &&) = default;
    PDB(const PDB &) = delete;
    PDB &operator=(PDB &&) = default;
    PDB &operator=(const PDB &) = delete;
    ~PDB() = default;

    /* Load the PDB by using its name (without extension) */
    bool Load(const char *pdb_name) {
      char pdb_fname[k_maxFileNameSize], abst_fname[k_maxFileNameSize];

      strcpy(pdb_fname, pdb_name);
      strcat(pdb_fname, ".pdb");
      strcpy(abst_fname, pdb_name);
      strcat(abst_fname, ".abst");

      printf("Reading pdb into memory: abst=%s", abst_fname);
      m_abstraction.reset(read_abstraction_from_file(abst_fname));

      printf(" pdb=%s\n", pdb_fname);
      FILE *pdb_file = fopen(pdb_fname, "r");
      assert(pdb_file != nullptr);
      m_map.reset(read_state_map(pdb_file));
      fclose(pdb_file);
      return true;
    }

    /* Get PDB Value from state. (asbtractStateHolder is used to avoid to
     * allocate many temp states, considered garbage for reuse after this call)
     */
    int GetValue(state_t *asbtractStateHolder, const state_t *state) {
      // compute PDB value for state
      abstract_state(m_abstraction.get(), state, asbtractStateHolder);
      const int *pdb_value = state_map_get(m_map.get(), asbtractStateHolder);
      assert(pdb_value != nullptr);
      return *pdb_value;
    }
  };

  /* State to avoid allocating temp states */
  state_t m_asbtractStateHolder;

  /* If we are using max (true) or addition (false) */
  bool m_useMax = true;

  /* PDBs loaded */
  std::vector<PDB> m_pdbs;
};

#endif // !PDB_HANDLER_HPP