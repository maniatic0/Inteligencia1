#pragma once
#ifndef READ_STATE_HPP
#define READ_STATE_HPP 1

#include <cstdio>

#define MAX_LINE_LENGTH 999

/* Read a State from stdin. Returns true if successful */
bool readState(state_t *state) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars;
  // READ A LINE OF INPUT FROM stdin
  printf("Please enter a state followed by ENTER: ");
  if (fgets(str, sizeof(str), stdin) == NULL) {
    printf("Error: empty input line.\n");
    return false;
  }

  // CONVERT THE STRING TO A STATE
  nchars = read_state(str, state);
  if (nchars <= 0) {
    printf("Error: invalid state entered.\n");
    return false;
  }

  printf("The state you entered is: ");
  print_state(stdout, state);
  printf("\n");
  return true;
}

#endif // !READ_STATE_HPP