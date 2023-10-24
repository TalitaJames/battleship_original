#ifndef RUNNER_H
#define RUNNER_H

typedef int ship;

struct board;
struct shipPosition;
struct hitmask;

enum cellStatus{
  UNKNOWN,
  MISS,
  HIT,
  SUNK,
};


const int BOARD_SIZE = 5;



board makeBoard(ship*, shipPosition*, short);
void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);

void printBoard(board);
void printHitmask(hitmask);

#endif // RUNNER_H