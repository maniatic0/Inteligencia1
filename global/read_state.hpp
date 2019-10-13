#pragma once
#ifndef READ_STATE_HPP
#define READ_STATE_HPP 1

#include <cstdio>

#define MAX_LINE_LENGTH 999

// #define DEBUG_PRINT 1 // Define this when importing to print debug info

/* Read a State from stdin. Returns true if successful */
bool readState(state_t *state) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars;
  // READ A LINE OF INPUT FROM stdin
#ifdef DEBUG_PRINT
  printf("Please enter a state followed by ENTER: ");
#endif // DEBUG_PRINT
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

#ifdef DEBUG_PRINT
  printf("The state you entered is: ");
  print_state(stdout, state);
  printf("\n");
#endif // DEBUG_PRINT

  return true;
}

#endif // !READ_STATE_HPP