#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"
#include "array_tools.h"
#include "merge_sort.h"

// === GLOBAL READONLY =====================================================

// DIM is the dimension of the Sudoku board
#define DIM 9

// DIMBOX is the dimension of a box
static const int DIMBOX = 3;
// EMPTY is the code for an empty cell
static const int EMPTY = 0;
// PRINT holds the characters for printing out the Sudoku board
static const char PRINT[] = { '_', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
// SUCCESS indicates the successful execution of a function
static const int SUCCESS = 0;
// CONSTRAINT_VIOLATED indicates that a row-, column, or box-constraint is
//   violated
static const int CONSTRAINT_VIOLATED = -1;

struct sudoku {
  // puzzle contains the start state of the puzzle
  int puzzle[DIM * DIM];
  // solution contains is equal to puzzle at the beginning and then stores
  //   all numbers filled in by the player.
  int solution[DIM * DIM];
};

// === HELPER FUNCTIONS =====================================================

// violate_row(su, row, num) returns CONSTRAINT_VIOLATED if num will violate 
//   the related row in su, and SUCCESS otherwise.
// requires: row is an integer between 0 and 8 (both inclusive)
// time: O(1)
// the function will run at most 9 iterations in the worsest case
static int violate_row(const struct sudoku *su, const int row, const int num) {
  assert(su);
  assert(0 <= row && row <= 8);

  // we check each element in row of su->solution to find whether there is a
  //   number that equals num
  for (int i = 0; i < DIM; ++i) {
    if (su->solution[row * DIM + i] == num) {
      return CONSTRAINT_VIOLATED;
    }
  }

  return SUCCESS;
}

// violate_col(su, col, num) returns CONSTRAINT_VIOLATED if num will violate 
//   the related column in su, and SUCCESS otherwise.
// requires: col is an integer between 0 and 8 (both inclusive)
// time: O(1)
// the function will run at most 9 iterations in the worsest case
static int violate_col(const struct sudoku *su, const int col, const int num) {
  assert(su);
  assert(0 <= col && col <= 8);

  // we check each element in col of su->solution to find whether there is a
  //   number that equals num
  for (int i = 0; i < DIM; ++i) {
    if (su->solution[i * DIM + col] == num) {
      return CONSTRAINT_VIOLATED;
    }
  }

  return SUCCESS;
}

// violate_box(su, row, col, num) returns CONSTRAINT_VIOLATED if num will 
//   violate the related box in su, and SUCCESS otherwise.
// requires: row is an integer between 0 and 8 (both inclusive)
//           col is an integer between 0 and 8 (both inclusive)
// time: O(1)
// the function will run at most 9 iterations in the worsest case
static int violate_box(const struct sudoku *su, const int row, const int col, 
                       const int num) {
  assert(su);
  assert(0 <= row && row <= 8);
  assert(0 <= col && col <= 8);

  // define and initialize a number to record the row and col number of the box
  int row_box = row / DIMBOX;
  int col_box = col / DIMBOX;

  // define and initialize the position of the left top number in box
  int row_pos = row_box * DIMBOX;
  int col_pos = col_box * DIMBOX;

  // we check each element in related box of su->solution to find whether 
  //   there is a number that equals num
  for (int i = row_pos; i < (row_box + 1) * DIMBOX; ++i) {
    for (int j = col_pos; j < (col_box + 1) * DIMBOX; ++j) {
      if (su->solution[i * DIM + j] == num) {
        return CONSTRAINT_VIOLATED;
      }
    }
  }

  return SUCCESS;
}

// least_possible_solutions(su, row, col, choices) determines all the possible 
//   values for each elements that do not violate any of the row-, column-, and
//   box- constraints. The function writes all possible values for the element
//   having the least possible solutions into choices, and modifies the row 
//   and col to be row and col of the element, and returns the number of 
//   choices or 0 if there is no such number.
// effects: might modify row and col
//          modify choices
// time: O(n)
// the function will check each elemetns with O(1) time two times which is
//   O(2*n) = O(n)
static int least_possible_solutions(struct sudoku *su, int *row, int *col, 
                                    int choices[]) {
  assert(su);
  assert(row);
  assert(col);
  assert(choices);

  // define and initialize a number to record the least possible solutions
  int least = DIM + 1;

  // check each element of su->solution and finds and compare the number of
  //   possible solution for each empty element
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (su->solution[i * DIM + j] == EMPTY) {
        // define and initialize a number to record number of choices, and
        //   modifies choices to record the possible solution
        int num_choices = cell_choices(su, i, j, choices);

        if (num_choices > 0 && num_choices < least) {
          least = num_choices;
        }
      }
    }
  }

  // check each element of su to find the positivn that can have the least
  //   number of possible solutions
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (su->solution[i * DIM + j] == EMPTY) {
        // define and initialize a number to record number of choices, and
        //   modifies choices to record the possible solution
        int num_choices = cell_choices(su, i, j, choices);

        if (num_choices == least) {
          *row = i;
          *col = j;
          return num_choices;
        }
      }
    }
  }

  return 0;
}

// === CORE FUNCTIONS =====================================================

// see sudoku.h for documentation
struct sudoku *sudoku_read(void) {
  struct sudoku *su = malloc(sizeof(struct sudoku));
  assert(su);
  int *pPuz = su->puzzle, *pSol = su->solution;
  for (int cnt = 0; cnt < DIM * DIM; ++cnt, ++pPuz, ++pSol) {
    char c = '\0';
    assert(scanf(" %c", &c) == 1);
    *pPuz = *pSol = (c == PRINT[EMPTY]) ? 0 : c - '0';
  }
  return su;
}

// see sudoku.h for documentation
void sudoku_destroy(struct sudoku *su) {
  assert(su);
  free(su);
}

// solution_print_separator() prints out a separator between boxes.
// produces output
static void solution_print_separator(void) {
  for (int box = 0; box < DIM / DIMBOX; ++box) {
    printf("+");
    for (int i = 0; i < 3 * DIMBOX; ++i) {
      printf("-");
    }
  }
  printf("+\n");
}

// see sudoku.h for documentation
void solution_print(const struct sudoku *su) {
  assert(su);

  for (int r = 0; r < DIM; ++r) {
    if (r % DIMBOX == 0) {
      solution_print_separator();
    }
    for (int c = 0; c < DIM; ++c) {
      if (c % DIMBOX == 0) {
        printf("|");
      }
      printf(" %c ", PRINT[su->solution[r * DIM + c]]);
    }
    printf("|\n");
  }
  solution_print_separator();
  printf("\n");
}

// see sudoku.h for documentation
void solution_reset(struct sudoku *su) {
  assert(su);
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      su->solution[row * DIM + col] = su->puzzle[row * DIM + col];
    }
  }  
}

// see sudoku.h for documentation
bool cell_erase(struct sudoku *su, int row, int col) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (su->puzzle[row * DIM + col] != EMPTY) {
    return false;
  } else {
    su->solution[row * DIM + col] = EMPTY;
    return true;
  }
}

// see sudoku.h for documentation
// time: O(1)
bool cell_fill(struct sudoku *su, int row, int col, int num) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);
  assert(1 <= num && num <= 9);

  if (su->puzzle[row * DIM + col] != EMPTY) {
    // if the number at the given position is already provided then the player
    //   cannot put the number at this position
    return false;
  } else {
    // we check whether violate any of the row-, column-, or box-constraints
    if (violate_row(su, row, num) || violate_col(su, col, num) || 
        violate_box(su, row, col, num)) {
      return false;
    } else {
      su->solution[row * DIM + col] = num;
      return true;
    }
  }
}

// see sudoku.h for documentation
// time: O(n)
bool puzzle_solved(const struct sudoku *su) {
  assert(su);

  // we then only check each elements of solution in su to see whether they are
  //   empty, since every time we use cell_fill we check whether the number
  //   we fill in the sudoku satisfies the row-, column-, or box-constraints,
  //   which means we do not need to check it here.
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      // define and initialize the number to record the current number we check
      int check_num = su->solution[i * DIM + j];

      if (check_num == EMPTY) {
        return false;
      }
    }
  }
  
  return true;
}

// see sudoku.h for documentation
// time: O(1)
int cell_choices(const struct sudoku *su, int row, int col, int choices[]) {
  assert(su);
  assert(choices);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (su->puzzle[row * DIM + col] != EMPTY) {
    // if the number at the given position is already provided then there is
    //   no choice to put any other number
    return 0;
  } else {
    // define and initialize a number to record the number of choice we have
    int choice = 0;

    // we check each number 1 to 9 to see how many choices do we have, and put
    //   the number into choices if it is valid
    for (int i = 1; i <= 9; ++i) {
      if (violate_row(su, row, i) || violate_col(su, col, i) || 
          violate_box(su, row, col, i)) {
        continue;
      } else {
        choices[choice] = i;
        ++choice;
      }
    }

    return choice;
  }
}

// see sudoku.h for documentation
// time: O(n)
bool cell_hint(const struct sudoku *su, int *row, int *col) {
  assert(su);
  assert(row);
  assert(col);

  // check each element of su->solution and finds a hint for an empty position,
  //   return true if there is only one choice for this position
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (su->solution[i * DIM + j] == EMPTY) {
        // define and initialize a array to record the possible choices
        int choices[DIM];

        // define and initialize a number to record number of choices, and
        //   modifies choices to record the possible solution
        int num_choices = cell_choices(su, i, j, choices);

        // if the number of choices is only 1, we modifies row and col to
        //   record the indices of the cell, and function returns true
        if (num_choices == 1) {
          *row = i;
          *col = j;
          return true;
        }
      }
    }
  }

  return false;
}

// see sudoku.h for documentation
// time: O(n^2)
bool sudoku_solve(struct sudoku *su) {
  assert(su);

  if (puzzle_solved(su)) {
    return true;
  } else {
    // define and initialize numbers to record the row and col of the current
    //   position we are looking at
    int row = 0;
    int col = 0;

    // define and initialize an array of integer to store the choices
    int choices[DIM];
    // define and initialize a number to record the least number of possible
    //   solutions
    int least_num = least_possible_solutions(su, &row, &col, choices);

    if (least_num == 0) {
      return false;
    } else {
      // verify all possible of the solutions
      for (int i = 0; i < least_num; ++i) {
        // define and initialize the current solution
        int curr_sol = choices[i];

        // fill in the cell with the solution
        if (!cell_fill(su, row, col, curr_sol)) {
          continue;
        }

        // run Sudokusolve recursively
        if (sudoku_solve(su)) {
          return true;
        }
      }

      // erase changes
      cell_erase(su, row, col);
      return false;
    }
  }
}
