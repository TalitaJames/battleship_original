#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = true;
  codeVersion = "v2.2";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  

  playGame(INFOGAIN, rndBoard());

  // hitmask hM;
  // probabilityGrid pG;
  // runThreads(hM, pG, threadCount);



  // std::ifstream myfile ("../out/workerSerialisation/test.txt");
  // std::string line; 
  // if (myfile.is_open()){
  //   int maxWorkers = 10;
  //   while (getline (myfile,line)){
  //     inputWorker(line);
  //   }
  //   myfile.close();
  // }
  // else std::cout << "ERROR! Unable to open file"; 

  return 0;
};

