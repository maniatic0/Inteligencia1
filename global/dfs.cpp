#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.hpp"

#define MAX_LINE_LENGTH 999

Node dfs_visit(std::shared_ptr<Node> node) {
  state = *node->GetState();
  currHistory = node->GetHistory();
  // If we finished
  if (is_goal(&state)) {
    return state;
  }
  // LOOP THROUGH SUCC
  init_fwd_iter(&iter, &state); // initialize the child iterator
  while ((ruleid = next_ruleid(&iter)) >= 0) {
    // Check for history pruning
    if (fwd_rule_valid_for_history(currHistory, ruleid) == 0) {
      continue;
    }
    // Generate Child
    apply_fwd_rule(ruleid, &state, &child);
    currHistory = next_fwd_history(currHistory, ruleid);

    std::shared_ptr<Node> m = dfs_visit(child);
    
    // We traverse
    if (m != nullptr) {
      return m;
    }
  }
  return nullptr;
}

int main(int argc, char **argv) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars;
  state_t state; // state_t is defined by the PSVN API. It is the type used for
                 // individual states.

  // VARIABLES FOR ITERATING THROUGH state's SUCCESSORS
  state_t child;
  ruleid_iterator_t iter; // ruleid_terator_t is the type defined by the PSVN
                          // API successor/predecessor iterators.
  int ruleid;             // an iterator returns a number identifying a rule
  int childHistory;       // Child history for pruning

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

  int currHistory = init_history;
  std::shared_ptr<Node> node = Node::CreateNode(state, nullptr, -1, currHistory);
  
  if (dfs_visit(node) == nullptr) {
    std::printf("Path not found\n");
    return 0;
  }

  // Print path
  PrintPath(node);
  node = nullptr;

  return 0;
}