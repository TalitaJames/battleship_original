#include <iostream>
#include <fstream>

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = false;
  codeVersion = "v3";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  std::string filename = "../out/workerSerialisation/test.txt";
  // std::string line;
  // std::ifstream myfile (filename);
  // if (myfile.is_open()){
  //   while (getline (myfile,line))
  //   {
  //     std::cout << line << '\n';
  //   }
  //   myfile.close();
  // } 
  // else
  //   std::cout << "Unable to open file"; 


  // Repeat testing
  std::vector<board> testBoards;

  for (size_t i = 0; i < 25; i++){
    board b = rndBoard();
    testBoards.push_back(b);
  }

  repeatIGRange(testBoards);

  return 0;
};

