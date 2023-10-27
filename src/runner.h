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
const int BOARD_DEFAULT = -1;



board makeBoard(ship*, shipPosition*, short);
void drawBoard(board &, ship*, shipPosition*);
void wipeBoard(board &);
board initBlankBoard(void);


void hitBoard(board, hitmask &, int, int);
bool checkCompatible(board,hitmask);

void printBoard(board);
void printHitmask(hitmask);

#endif // RUNNER_H