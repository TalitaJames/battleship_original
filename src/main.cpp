#include <iostream> 

#include "boatsAndBoards.h"
#include "json/json.h"
#include "mcTreesearch.h"


int main() {
  verbose = true;
  codeVersion = "v2.2";
  std::cout<<"Board Len: "<< BOARD_SIZE<<"\tFleet size: "<< FLEET_SIZE<<"\tthreadCount: "<<threadCount<<"\tverbose: "<<verbose<<std::endl;
  

  // std::string line;
  // std::ifstream file("../out/workerSerialisation/test.txt");
  // if (file.is_open()) {
  //   while (getline(file, line)) {
  //     std::cout << line << "\n";
  //   }
  //   file.close();
  // }

  // std::string line;
  // std::ifstream myfile ("../out/workerSerialisation/test.txt");
  // if (myfile.is_open()){
  //   int maxWorkers = 10;
  //   while (getline (myfile,line)){
  //     std::cout << "IN " << line << '\n';
  //   }
  //   myfile.close();
  // }
  // else std::cout << "ERROR! Unable to open file"; 


  // worker outW;
  // intToShipArray(126, outW.end); 
  // std::cout<< outW << std::endl;

  // inW >> "0,22";
  // "0,22" >> inW;
  // std::cout<< inW << "\n";
  // printWorkers({inW, outW});

  hitmask hM;
  probabilityGrid pG;
  runThreads(hM, pG, threadCount);
  // repeatGames({FLEXI}, 2, false);

  return 0;
};

