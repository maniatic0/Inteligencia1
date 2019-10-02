#include <assert.h>
#include <inttypes.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>

#include "node.hpp"
#include "priority_queue.hpp"

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

  // Setup. Put Start node in queue to open, mark it as black and put its cost to 0
  state_map_add(color_map, &state, static_cast<int>(Color::Gray));
  state_map_add(cost_map, &state, 0);
  std::shared_ptr<Node> node = Node::CreateNode(state, nullptr, -1);
  open.Add(0, 0, node);
  node = nullptr;

  int parent_cost = -1;
  int child_new_cost = -1;

  // While we have something to open
  while (!open.Empty()) {

    // Get the top node
    parent_cost = open.CurrentPriority();
    node = open.Top();
    open.Pop();
    state = *node->GetState();

    // If we finished
    if (is_goal(&state)) {
      found = true;
      break;
    }

    // LOOP THOUGH THE CHILDREN ONE BY ONE
    init_fwd_iter(&iter, &state); // initialize the child iterator
    while ((ruleid = next_ruleid(&iter)) >= 0) {

      // Generate Child
      apply_fwd_rule(ruleid, &state, &child);

      child_new_cost = parent_cost + get_fwd_rule_cost(ruleid);

      // Child Color
      const int *child_color = state_map_get(color_map, &child);
      const int *child_cost = state_map_get(cost_map, &child);
      // We haven't seen it
      if (child_color == nullptr ||
          static_cast<Color>(*child_color) == Color::White) {
        // add to open with color gray
        state_map_add(color_map, &child, static_cast<int>(Color::Gray));
        // Update Cost
        state_map_add(cost_map, &child, child_new_cost);
        // Add with priority
        open.Add(child_new_cost, child_new_cost, node->MakeNode(child, ruleid));
      } else if (child_new_cost < *child_cost) {
        // update open child
        assert(static_cast<Color>(*child_color) == Color::Gray);
        // Update Cost
        state_map_add(cost_map, &child, child_new_cost);
        // Add with priority
        open.Add(child_new_cost, child_new_cost, node->MakeNode(child, ruleid));
      }
    }

    // Get State of the current node and mark it black
    state = *node->GetState();
    state_map_add(color_map, &state, static_cast<int>(Color::Black));
  }

  if (!found) {
    std::printf("Path not found\n");
    return 0;
  }

  // Print path
  PrintPath(node);

  return 0;
}