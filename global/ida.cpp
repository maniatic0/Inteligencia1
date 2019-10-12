#include <assert.h>
#include <cstdint>
#include <inttypes.h>
#include <limits>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <utility>


#include "node.hpp"
#include "read_state.hpp"

long long unsigned heuristic(const state_t *from);

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
  if (node->GetCost() > bound) {
    return std::make_pair<std::shared_ptr<Node>, long long unsigned>(
        std::shared_ptr<Node>(nullptr), node->GetCost());
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

    calc = ida(node->MakeNode(child, ruleid, childHistory,
                              heuristic(node->GetState())));

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

int main() {

  state_t state;
  if (!readState(&state)) {
    return -1;
  }

  int currHistory = init_history;
  std::shared_ptr<Node> node =
      Node::CreateNode(state, nullptr, -1, currHistory);

  while (true) {
    calc = ida(node);

    if (calc.first != nullptr) {
      break;
    }
    bound += calc.second;
  }

  // Print path
  PrintPath(calc.first);
}