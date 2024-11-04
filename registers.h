#ifndef REGISTERS_H
#define REGISTERS_H

#include <iostream>
#include <string>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct Z80{

        //REGISTERS--------------------------------
        //General 8-bit registers
        uint8_t *regA;
        uint8_t *Flags; //Single Byte
        uint8_t *regB;
        uint8_t *regC;
        uint8_t *regD;
        uint8_t *regE;
        uint8_t *regH;
        uint8_t *regL;
        //Alternate 8-bit registers
        uint8_t *reg_A; 
        uint8_t *reg_F;
        uint8_t *reg_B;
        uint8_t *reg_C;
        uint8_t *reg_D;
        uint8_t *reg_E;
        uint8_t *reg_H;
        uint8_t *reg_L;

        //Weird 8-bit registers
        uint8_t *reg_I; //Interrupt Page Address
        uint8_t *reg_R; //Memory Refresh

        //16-bit registers
        uint16_t *reg_IX;
        uint16_t *reg_IY;
        uint16_t *reg_SP;//Stack Pointer 
        uint16_t *reg_PC;//Program Counter

        //Paired Registers - ??DOES LITTLE ENDIAN APPLY HERE?? if ading A to 
        uint8_t *regBC[2] = {regB, regC};
        uint8_t *regDE[2] = {regD, regE};
        uint8_t *regHL[2] = {regH, regL};
        uint8_t *reg_BC[2] = {reg_B, reg_C};
        uint8_t *reg_DE[2] = {reg_D, reg_E};
        uint8_t *reg_HL[2] = {reg_H, reg_L};

        //Z80 instance constructor
        Z80 RegInit(){

                //General 8-bit registers
                *regA = NULL;
                *Flags = NULL; //Single Byte
                *regB = NULL;
                *regC = NULL;
                *regD = NULL;
                *regE = NULL;
                *regH = NULL;
                *regL = NULL;

                //Alternate 8-bit registers
                *reg_A = NULL; 
                *reg_F = NULL;
                *reg_B = NULL;
                *reg_C = NULL;
                *reg_D = NULL;
                *reg_E = NULL;
                *reg_H = NULL;
                *reg_L = NULL;

                //Weird 8-bit registers
                *reg_I = NULL; //Interrupt Page Address
                *reg_R = NULL; //Memory Refresh

                //16-bit registers
                *reg_IX = NULL;
                *reg_IY = NULL;
                *reg_SP = NULL;//Stack Pointer 
                *reg_PC = NULL;//Program Counter

        }

};

#endif