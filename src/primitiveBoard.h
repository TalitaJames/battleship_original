#ifndef PRIMITIVEBOARD_H
#define PRIMITIVEBOARD_H

#include <bitset>
#include <string>
#include <cstdint>
#include <map>

class PrimitiveBoard
{
private:
    bool isBad_; 
    std::bitset<7> bitBoard_; //FIXME: make a variable sized bitboard

public:
    PrimitiveBoard(std::uint8_t shipCodes[], int fleet[]);
    PrimitiveBoard(std::uint8_t shipCodes[], int fleet[], std::map<int, bool> hitmask);
    ~PrimitiveBoard();

    std::string to_string();
    bool getIsBad(void);

};




#endif  // PRIMITIVEBOARD_H
