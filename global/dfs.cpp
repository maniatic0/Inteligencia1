#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.hpp"

#define MAX_LINE_LENGTH 999

/* Avoid save useless stuff in stack (we don't save them because we can
 * regenerate them) */
int ruleid;       // Generated Rule id
state_t child;    // Generated Child
int childHistory; // Generated Child History

std::shared_ptr<Node> dfs_visit(std::shared_ptr<Node> node) {
  // If we finished
  if (is_goal(node->GetState())) {
    return node;
  }

  ruleid_iterator_t iter;  // ruleid_terator_t is the type defined by the PSVN
  std::shared_ptr<Node> m; //

  // LOOP THROUGH SUCC
  init_fwd_iter(&iter, node->GetState()); // initialize the child iterator
  while ((ruleid = next_ruleid(&iter)) >= 0) {
    // Check for history pruning
    if (fwd_rule_valid_for_history(node->GetHistory(), ruleid) == 0) {
      continue;
    }
    // Generate Child
    apply_fwd_rule(ruleid, node->GetState(), &child);
    childHistory = next_fwd_history(node->GetHistory(), ruleid);

    m = dfs_visit(node->MakeNode(child, ruleid, childHistory));

    // We traverse
    if (m != nullptr) {
      return m;
    }
    m = nullptr;
  }
  return nullptr;
}

int main(int argc, char **argv) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars;
  state_t state; // state_t is defined by the PSVN API. It is the type used for
                 // individual states.

  // READ A LINE OF INPUT FROM stdin
  printf("Please enter a state followed by ENTER: ");
  if (fgets(str, sizeof(str), stdin) == NULL) {
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
  std::shared_ptr<Node> node =
      Node::CreateNode(state, nullptr, -1, currHistory);

  if ((node = dfs_visit(node)) == nullptr) {
    std::printf("Path not found\n");
    return 0;
  }

  // Print path
  PrintPath(node);

  return 0;
}