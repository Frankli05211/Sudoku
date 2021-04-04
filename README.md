# Sudoku
# A sudoku board can be solve by this program. The user can firstly create any sudoku board, for example,
8________
___26__5_
__98____4
7_3____2_
____4__81
5____7_3_
_5___924_
___7_____
91_______

Then the user is able to use the following command:
  "quit": quit the game, and stop the program
  "reset": restart the game with the same sudoku board
  "print": print the current game board
  "fill n1 n2 n": put the number n at row n1 and column n2
  "erase n1 n2": erase the number at row n1 and column n2
  "choices n1 n2": produce all choices for the location row n1 and column n2 to the output stream
  "hint n1 n2": produce a hint at row n1 and column n2 to the output stream
  "solve": the function will automatically try to solve the sudoku using search and backtracking, if the given sudoku can be solved, function will return the final answer, otherwise function will produce an error message to the output stream
