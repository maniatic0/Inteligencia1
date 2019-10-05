#include <assert.h>
#include <cstdint>
#include <inttypes.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>

#include "node.hpp"
#include "stopwatch.hpp"

/* Avoid sending constants through stack */
unsigned bound = 0;
/* Avoid save useless stuff in stack (we don't save them because we can
 * regenerate them) */
int ruleid;       // Generated Rule id
state_t child;    // Generated Child
int childHistory; // Generated Child History

long long unsigned brachADA(std::shared_ptr<Node> node, unsigned depth) {
  // Nodes just after the bound
  if (depth >= bound) {
    return 1llu;
  }

  long long unsigned generated = 0llu;
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

    generated +=
        brachADA(node->MakeNode(child, ruleid, childHistory), depth + 1llu);
  }
  return generated;
}

int main() {
  state_t state;
  int d;
  first_goal_state(&state, &d);
  // Note, this only works for single goal problems. It would be complicated to
  // handle the stopping between them

  int currHistory = init_history;
  std::shared_ptr<Node> node =
      Node::CreateNode(state, nullptr, -1, currHistory);

  long long unsigned prevNodeAmount = 1llu;
  long long unsigned currNodeAmount;

  bound = 0;

  Stopwatch::Stopwatch watch(1, false);

  Stopwatch::ms::rep time;

  while (true) {
    ++bound;
    watch.Start();
    currNodeAmount = brachADA(node, 0);
    time = watch.Stop<Stopwatch::ms>();
    std::printf("depth=%u", bound);
    std::printf(" generated=%" PRIu64, currNodeAmount);
    std::printf(" factor=%f", static_cast<double>(currNodeAmount) /
                                  static_cast<double>(prevNodeAmount));
    std::printf(" elapsed=%" PRIu64 "ms", time);
    std::printf(" nodesPerMs=%f\n", static_cast<double>(currNodeAmount) /
                                        static_cast<double>(time));

    prevNodeAmount = currNodeAmount;
  }
}