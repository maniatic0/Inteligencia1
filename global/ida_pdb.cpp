#include <assert.h>
#include <cstdint>
#include <inttypes.h>
#include <limits>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <utility>

//#define DEBUG_PRINT 1 // For Debug printing

#include "node.hpp"
#include "pdb_handler.hpp"
#include "read_state.hpp"
#include "stopwatch.hpp"

PDBHandler pdbs;

long long unsigned heuristic(const state_t *from) {
  return pdbs.GetValue(from);
}

// How many nodes were generated
long long unsigned generated = 0;

/* Avoid sending constants through stack */
long long unsigned bound = 0;
/* Avoid save useless stuff in stack (we don't save them because we can
 * regenerate them) */
int ruleid;       // Generated Rule id
state_t child;    // Generated Child
int childHistory; // Generated Child History
std::pair<std::shared_ptr<Node>, long long unsigned>
    calc; // Calculation made by ida

std::pair<std::shared_ptr<Node>, long long unsigned>
ida(std::shared_ptr<Node> node) {
  // Nodes just after the bound
  if (node->GetCostHeuristic() > bound) {
    return std::make_pair<std::shared_ptr<Node>, long long unsigned>(
        std::shared_ptr<Node>(nullptr), node->GetCostHeuristic());
  }

  if (is_goal(node->GetState())) {
    long long unsigned cost = node->GetCost();
    return std::make_pair(std::move(node), cost);
  }

  long long unsigned minFailedCost =
      std::numeric_limits<long long unsigned>::max();
  ruleid_iterator_t iter; // ruleid_terator_t is the type defined by the PSVN

  // LOOP THOUGH THE CHILDREN ONE BY ONE
  init_fwd_iter(&iter, node->GetState()); // initialize the child iterator
  while ((ruleid = next_ruleid(&iter)) >= 0) {

    // Check for history pruning
    if (fwd_rule_valid_for_history(node->GetHistory(), ruleid) == 0) {
      continue;
    }

    // Generate Child
    apply_fwd_rule(ruleid, node->GetState(), &child);
    childHistory = next_fwd_history(node->GetHistory(), ruleid);

    ++generated;
    calc = ida(node->MakeNode(child, ruleid, childHistory,
                              heuristic(&child)));

    // found it
    if (calc.first != nullptr) {
      return calc;
    }

    // failed to find it
    if (calc.second < minFailedCost) {
      minFailedCost = calc.second;
    }
  }
  return std::make_pair(std::shared_ptr<Node>(nullptr), minFailedCost);
}

int main(int argc, const char **argv) {

  if (!pdbs.Load(argc, argv, 1)) {
    return -1;
  }

  state_t state;
  if (!readState(&state)) {
    return -1;
  }

  Stopwatch::Stopwatch watch(1, false);

  Stopwatch::sec::rep time_used;

  watch.Start();

  int currHistory = init_history;
  std::shared_ptr<Node> node =
      Node::CreateNode(state, nullptr, -1, currHistory);

  bound = heuristic(node->GetState());

#ifdef DEBUG_PRINT
  printf("Heuristic " "%" PRIu64 "\n", bound);
#endif // DEBUG_PRINT

  while (true) {
#ifdef DEBUG_PRINT
    printf("%" PRIu64 " ", bound);
    fflush(stdout);
#endif // DEBUG_PRINT
    calc = ida(node);

    if (calc.first != nullptr) {
      break;
    }
    bound = calc.second;
  }

#ifdef DEBUG_PRINT
  printf("\n");
#endif // DEBUG_PRINT

  time_used = watch.Stop<Stopwatch::sec>();

  print_state(stdout, node->GetState());
  std::printf("generated=%" PRIu64, generated);
  std::printf(" cost=%" PRIu64, calc.second);
  std::printf(" time=%" PRIu64 "s", time_used);
  std::printf(" nodesPerSec=%f\n",
              static_cast<double>(generated) / static_cast<double>(time_used));
#ifdef DEBUG_PRINT
  // Print path
  PrintPath(calc.first);
#endif // DEBUG_PRINT
  return 0;
}