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
        uint8_t regA = 0; //Accumulator
        uint8_t Flags = 0; //Single Byte
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
        uint16_t reg_IX = 0;
        uint16_t reg_IY = 0;
        uint16_t reg_SP = 0;//Stack Pointer 
        uint16_t reg_PC = 0;//Program Counter

        //Paired Registers - ??DOES LITTLE ENDIAN APPLY HERE?? if ading A to 
        uint8_t regBC[2] = {regB, regC};
        uint8_t regDE[2] = {regD, regE};
        uint8_t regHL[2] = {regH, regL};
        uint8_t reg_BC[2] = {reg_B, reg_C};
        uint8_t reg_DE[2] = {reg_D, reg_E};
        uint8_t reg_HL[2] = {reg_H, reg_L};

        //Holds the number of cycles performed
        int cycleCnt;


//I BELIEVE THAT THIS CODE IS UNECESSARY BUT JUST IN CASE IT ISNT, I PUT IT IT HERE 

//==================================================================================================================================================


        // //Initalizes everything to 0;
        // void init(){

        //         //REGISTERS--------------------------------
        //         //General 8-bit registers
        //         uint8_t *regA = nullptr;
        //         uint8_t *Flags = nullptr; //Single Byte
        //         uint8_t *regB = nullptr;
        //         uint8_t *regC = nullptr;
        //         uint8_t *regD = nullptr;
        //         uint8_t *regE = nullptr;
        //         uint8_t *regH = nullptr;
        //         uint8_t *regL = nullptr;
                
        //         //Alternate 8-bit registers
        //         uint8_t *reg_A = nullptr; 
        //         uint8_t *reg_F = nullptr;
        //         uint8_t *reg_B = nullptr;
        //         uint8_t *reg_C = nullptr;
        //         uint8_t *reg_D = nullptr;
        //         uint8_t *reg_E = nullptr;
        //         uint8_t *reg_H = nullptr;
        //         uint8_t *reg_L = nullptr;

        //         //Weird 8-bit registers
        //         uint8_t *reg_I = nullptr; //Interrupt Page Address
        //         uint8_t *reg_R = nullptr; //Memory Refresh

        //         //16-bit registers
        //         uint16_t *reg_IX = nullptr;
        //         uint16_t *reg_IY = nullptr;
        //         uint16_t *reg_SP = nullptr;//Stack Pointer 
        //         uint16_t *reg_PC = 0;//Program Counter
        //         //Current cycle count
        //         int cycleCnt = 0;

        // }


//==================================================================================================================================================

};

#endif