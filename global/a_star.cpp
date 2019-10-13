#include <assert.h>
#include <inttypes.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>

#include "node.hpp"
#include "priority_queue.hpp"

long long unsigned heuristic(const state_t *from);

#define MAX_LINE_LENGTH 999

int main(int argc, char **argv) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars;
  state_t state; // state_t is defined by the PSVN API. It is the type used for
                 // individual states.

  // VARIABLES FOR ITERATING THROUGH state's SUCCESSORS
  bool found = false;
  state_t child;
  ruleid_iterator_t iter; // ruleid_terator_t is the type defined by the PSVN
                          // API successor/predecessor iterators.
  int ruleid;             // an iterator returns a number identifying a rule
  int childHistory;       // Child history for pruning

  PriorityQueue<std::shared_ptr<Node>>
      open; // used for the states we have generated but not
            // yet expanded (the OPEN list)
  state_map_t *color_map = new_state_map(); // contains the color of all states
                                            // that have been generated
  state_map_t *cost_map = new_state_map();  // contains the cost-to-goal for all
                                            // states that have been generated

  // Child Color
  const int *child_color;

  // READ A LINE OF INPUT FROM stdin
  printf("Please enter a state followed by ENTER: ");
  if (fgets(str, sizeof str, stdin) == NULL) {
    printf("Error: empty input line.\n");
    return -1;
  }

  // CONVERT THE STRING TO A STATE
  nchars = read_state(str, &state);
  if (nchars <= 0) {
    printf("Error: invalid state entered.\n");
    return -1;
  }

  printf("The state you entered is: ");
  print_state(stdout, &state);
  printf("\n");

  // Setup. Put Start node in queue to open, mark it as black and put its cost
  // to 0
  state_map_add(color_map, &state, static_cast<int>(Color::Gray));
  int currHistory = init_history;
  std::shared_ptr<Node> node =
      Node::CreateNode(state, nullptr, -1, currHistory);
  open.Add(0, 0, std::move(node));
  node = nullptr;

  int child_new_cost = -1;
  int heuristic_child_cost = -1;

  // While we have something to open
  while (!open.Empty()) {

    // Get the top node
    node = open.Top();
    open.Pop();

    const int *node_best_cost = state_map_get(cost_map, node->GetState());

    if (node_best_cost != nullptr &&
        node->GetCost() >= static_cast<unsigned long long>(*node_best_cost)) {
      // Delayed duplicate elimination
      continue;
    }

    state_map_add(cost_map, node->GetState(), node->GetCost());

    // If we finished
    if (is_goal(node->GetState())) {
      found = true;
      break;
    }

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
      heuristic_child_cost = static_cast<int>(heuristic(&child));
      child_new_cost = node->GetCost() + get_fwd_rule_cost(ruleid);

      state_map_add(color_map, &child, static_cast<int>(Color::Gray));
      open.Add(
          child_new_cost + heuristic_child_cost, child_new_cost,
          node->MakeNode(child, ruleid, childHistory, heuristic_child_cost));
    }

    // Get State of the current node and mark it black
    state_map_add(color_map, node->GetState(), static_cast<int>(Color::Black));
  }

  if (!found) {
    std::printf("Path not found\n");
    return 0;
  }

  // Print path
  PrintPath(node);

  return 0;
}