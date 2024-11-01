#include <iostream>
#include <string>
#include <cstdint>
using namespace std;

class Z80{
    private:

        //REGISTERS--------------------------------
        //General 8-bit registers
        uint8_t regA;
        uint8_t Flags; //Single Byte
        uint8_t regB;
        uint8_t regC;
        uint8_t regD;
        uint8_t regE;
        uint8_t regH;
        uint8_t regL;
        //Alternate 8-bit registers
        uint8_t reg_A; 
        uint8_t reg_F;
        uint8_t reg_B;
        uint8_t reg_C;
        uint8_t reg_D;
        uint8_t reg_E;
        uint8_t reg_H;
        uint8_t reg_L;

        //Weird 8-bit registers
        uint8_t reg_I; //Interrupt Page Address
        uint8_t reg_R; //Memory Refresh

        //16-bit registers
        uint16_t reg_IX;
        uint16_t reg_IY;
        uint16_t reg_SP;//Stack Pointer 
        uint16_t reg_PC;//Program Counter

        //Paired Registers - ??DOES LITTLE ENDIAN APPLY HERE??
        uint8_t regBC[2] = {regB, regC};
        uint8_t regDE[2] = {regD, regE};
        uint8_t regHL[2] = {regH, regL};
        uint8_t reg_BC[2] = {reg_B, reg_C};
        uint8_t reg_DE[2] = {reg_D, reg_E};
        uint8_t reg_HL[2] = {reg_H, reg_L};

        //MEMORY SPACE ----------------------------------------------
        int memory[512000]; //??Holds the read in opcode from binary??

    public: 

};

int main(){

    cout << "poop in my butt emma style" << endl;

}