#ifndef REGISTERS_H
#define REGISTERS_H

#include <iostream>
#include <string>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct Z80{

        // https://jnz.dk/z80/flags.html

        /*
        how the flags work for max's reference 
        
        bit position(in binary)     ->   7  |6  |5  |4  |3  |2  |1  |0
        flag                        ->   S  |Z  |x  |H  |y  |P/V|N  |C

        C == carry flag -> 0x00 - 0x01
        N == add/subtract -> 0x02
        P/V == parity/overflow flag -> 0x04
        y == undoccumented bit - 3rd bit of the result
        H == half carry flag -> 0x08
        x == undoccumented bit - 5th bit of the result
        Z == zero flag -> 0x40
        S == sign flag  -> 0x80

        */

        //REGISTERS--------------------------------
        //General 8-bit registers
        uint8_t regA = 0; //Accumulator
        uint8_t Flags = 0x40; //Single Byte - Initialization from ex outputs provided.
        uint8_t regB = 0;
        uint8_t regC = 0;
        uint8_t regD = 0;
        uint8_t regE = 0;
        uint8_t regH = 0;
        uint8_t regL = 0;
        //Alternate 8-bit registers
        uint8_t reg_A = 0; 
        uint8_t reg_F = 0; 
        uint8_t reg_B = 0;
        uint8_t reg_C = 0;
        uint8_t reg_D = 0;
        uint8_t reg_E = 0;
        uint8_t reg_H = 0;
        uint8_t reg_L = 0;

        //Weird 8-bit registers
        uint8_t reg_I = 0; //Interrupt Page Address
        uint8_t reg_R = 0; //Memory Refresh

        //16-bit registers
        uint16_t reg_IX = 0xffff; //Initialization from ex outputs provided.
        uint16_t reg_IY = 0xffff; //Initialization from ex outputs provided.
        uint16_t reg_SP = 0;//Stack Pointer 
        uint16_t reg_PC = 0;//Program Counter

        //UNDOCCUMENTED REGISTERS
        uint8_t regIFF1 = 0; //Interrupt Flip Flop 1
        uint8_t regIFF2 = 0; //Interrupt Flip Flop 2
        uint8_t regINIR = 0; //Internal 

        //Paired Registers
        uint8_t regBC[2] = {regB, regC};
        uint8_t regDE[2] = {regD, regE};
        uint8_t regHL[2] = {regH, regL};
        uint8_t reg_BC[2] = {reg_B, reg_C};
        uint8_t reg_DE[2] = {reg_D, reg_E};
        uint8_t reg_HL[2] = {reg_H, reg_L};

        //Holds the number of cycles performed
        int cycleCnt;

};

#endif