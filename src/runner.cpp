#include <iostream> 
#include "primitiveBoard.h"

// struct{
//   int fleet[];
//   std::map<int, bool> hitmask;

// } gameState;




int main() { //main function
  std::cout << "Hello World \n";

  std::uint8_t shipCodes[]={0x0, 0x0, 0x0, 0x0, 0x0};
  int fleet[] = {2,3,3,4,5};

  PrimitiveBoard foo(shipCodes, fleet);

  return 0;
}