/*
Max Castle and Emma Chaney's Z80 Emmulator for CMSC 411 
*/

//INCLUDES===================================================================================================================
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <bitset>
#include <cmath>
#include <bit>
#include "registers.h"
using namespace std;

//FILE NAMES FOR RUNNING===================================================================================================
const string filenameEMMA = "C:\\Users\\ekcha\\OneDrive\\Documents\\GitHub\\411-paper\\simple-sub.bin";
const string filenameMAX = "C:\\411\\rotate.bin"; //MAX, PUT .BIN AFTER THE GODDAMN OATH NAME
const string fileRun = filenameMAX;


//DEFINING IMPORTNANT THINGS=======================================================================================================
int const CYCLES = 1024;
int const MEMSIZE = 65536;
bool INTERUPT_MODE1 = true;
bool INTERUPT_MODE2 = true;
static uint8_t memory[MEMSIZE];
Z80 cpu;


//FUNCTION DEFINITIONS=========================================
int decode();
void printReg(Z80);
void printRegTest(Z80);
uint8_t addFlags(uint8_t, uint8_t);
uint8_t subFlags(uint8_t, uint8_t);
uint8_t incFlags(uint8_t);
uint8_t twosComp(uint8_t);
void andFlags(uint8_t);
void xorFlags(uint8_t);
uint16_t incPaired(uint8_t, uint8_t);
uint16_t add16Flags(uint8_t, uint8_t, uint16_t);
uint8_t decFlags(uint8_t);
uint16_t decPaired(uint8_t, uint8_t);
uint8_t adcFlags(uint8_t, uint8_t);
uint8_t sbcFlags(uint8_t, uint8_t);
void retS();
uint16_t popS();

//OUT OF SIGHT OUT OF MIND (DONT TOUCH THESE I DIDNT WRITE THEM)====================================================================
void z80_mem_write(uint16_t addr, uint8_t value) {
    memory[addr] = value;
}

uint8_t z80_mem_read(uint16_t addr) {
    return memory[addr];
}

//i dont know what this is used for
void z80_mem_write16(uint16_t addr, uint16_t value) {
    memory[addr] = (uint8_t)(value & 0xff);
    memory[addr + 1] = (uint8_t)(value >> 8);
}

uint16_t z80_mem_read16(uint16_t addr) {
    return ((uint16_t)memory[addr]) | (((uint16_t)memory[addr + 1]) << 8);
}

void z80_mem_dump(const char *filename) {
    FILE *fileptr;

    if(!(fileptr = fopen(filename, "wb"))) {
        fprintf(stderr, "z80_mem_dump: Cannot open destination file %s: %s\n",
                filename, strerror(errno));
        return;
    }

    if(fwrite(memory, 1, 65536, fileptr) != 65536) {
        cout << "z80_mem_dump: Couldn't write full memory dump" << endl;
        fclose(fileptr);
        return;
    }

    fclose(fileptr);
    cout << "Successfully Dumped Z80 Memory to memory.bin :)" << endl;
}

void z80_mem_load(const char *filename) {
    FILE *fileptr;

    if(!(fileptr = fopen(filename, "rb"))) {
        cout << "Cannot read" << endl;
        return;
    }

    if(!fread(memory, 1, 65536, fileptr)) {
        fprintf(stderr, "z80_mem_read: Couldn't read progmemory binary\n");
        fclose(fileptr);
        return;
    }

    fclose(fileptr);
    return;
}

//FLAG TABLES - PROVIDED BY SEBALD=========================================================================================================
//HOW TO USE:   The flags listed in the table name will be changed by said table when you put in the result into []. 
//              You MUST |= any FLAGS NOT LISTED - whether they are unaffected or you are changing them manually.

static const uint8_t ZStable[256] =
{
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};

static const uint8_t ZSXYtable[256] =
{
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
    0x28, 0x28, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8
};

static const uint8_t ZSPXYtable[256] =
{
    0x44, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x08, 0x0C,
    0x0C, 0x08, 0x0C, 0x08, 0x08, 0x0C, 0x00, 0x04, 0x04, 0x00,
    0x04, 0x00, 0x00, 0x04, 0x0C, 0x08, 0x08, 0x0C, 0x08, 0x0C,
    0x0C, 0x08, 0x20, 0x24, 0x24, 0x20, 0x24, 0x20, 0x20, 0x24,
    0x2C, 0x28, 0x28, 0x2C, 0x28, 0x2C, 0x2C, 0x28, 0x24, 0x20,
    0x20, 0x24, 0x20, 0x24, 0x24, 0x20, 0x28, 0x2C, 0x2C, 0x28,
    0x2C, 0x28, 0x28, 0x2C, 0x00, 0x04, 0x04, 0x00, 0x04, 0x00,
    0x00, 0x04, 0x0C, 0x08, 0x08, 0x0C, 0x08, 0x0C, 0x0C, 0x08,
    0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x00, 0x08, 0x0C,
    0x0C, 0x08, 0x0C, 0x08, 0x08, 0x0C, 0x24, 0x20, 0x20, 0x24,
    0x20, 0x24, 0x24, 0x20, 0x28, 0x2C, 0x2C, 0x28, 0x2C, 0x28,
    0x28, 0x2C, 0x20, 0x24, 0x24, 0x20, 0x24, 0x20, 0x20, 0x24,
    0x2C, 0x28, 0x28, 0x2C, 0x28, 0x2C, 0x2C, 0x28, 0x80, 0x84,
    0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x8C, 0x88, 0x88, 0x8C,
    0x88, 0x8C, 0x8C, 0x88, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84,
    0x84, 0x80, 0x88, 0x8C, 0x8C, 0x88, 0x8C, 0x88, 0x88, 0x8C,
    0xA4, 0xA0, 0xA0, 0xA4, 0xA0, 0xA4, 0xA4, 0xA0, 0xA8, 0xAC,
    0xAC, 0xA8, 0xAC, 0xA8, 0xA8, 0xAC, 0xA0, 0xA4, 0xA4, 0xA0,
    0xA4, 0xA0, 0xA0, 0xA4, 0xAC, 0xA8, 0xA8, 0xAC, 0xA8, 0xAC,
    0xAC, 0xA8, 0x84, 0x80, 0x80, 0x84, 0x80, 0x84, 0x84, 0x80,
    0x88, 0x8C, 0x8C, 0x88, 0x8C, 0x88, 0x88, 0x8C, 0x80, 0x84,
    0x84, 0x80, 0x84, 0x80, 0x80, 0x84, 0x8C, 0x88, 0x88, 0x8C,
    0x88, 0x8C, 0x8C, 0x88, 0xA0, 0xA4, 0xA4, 0xA0, 0xA4, 0xA0,
    0xA0, 0xA4, 0xAC, 0xA8, 0xA8, 0xAC, 0xA8, 0xAC, 0xAC, 0xA8,
    0xA4, 0xA0, 0xA0, 0xA4, 0xA0, 0xA4, 0xA4, 0xA0, 0xA8, 0xAC,
    0xAC, 0xA8, 0xAC, 0xA8, 0xA8, 0xAC
};

//EXECUTION CODES==================================================================================================================
//Run loop
int z80_execute(){

    while(cpu.cycleCnt < CYCLES)
    {
        cpu.reg_R++; //INCRAMENT THE R RESGISTER ON ALL PREFIXES EXCEPT IY CB IX CB
        if(decode()) {break;} // if decode returns 1 (either halt or unknown inst), break
                              // otherwise, keep looping. 
    }
    cout << "cycles completed: [" << cpu.cycleCnt << "]\n" << endl;
    
    return cpu.cycleCnt; // return CompletedCylces;
}

//Determine which instruction to run and execute it
int decode()
{
    uint8_t inst = z80_mem_read(cpu.reg_PC++); //Reads the next instuction and incraments program counter
    switch(inst)
    {

        //HALT INSTRUCTION==========================================================================================
        case 0x76: //HALT INSTRUCTION - print out all the registers and dump memory to .bin file
            
            if(cpu.regIFF1 && cpu.regIFF2)
            {
                cpu.cycleCnt += 4; // 4 Cycles for halt
                //printReg(cpu); //OUR VERSION OF PRINT
                printRegTest(cpu); //SEBALD VERSION OF PRINT
                z80_mem_dump("memory.bin");
                return 1;
            }

            return 0;
            
            break;

        //NOP INSTUCTION===========================================================================================
        case 0x00: //NOP INSTRUCTION
            cpu.cycleCnt += 4;
            break;
        
        //MODE CHANGE INSTRUCTIONS================================================================================
    {
        //EXCHANGE INSTUCTIONS-----------------------------------
        case 0x08: //EXCHANGE INSTRUCTION - swap af with af'
        {
            int8_t temp = cpu.regA;
            int8_t temp2 = cpu.Flags;
            cpu.regA = cpu.reg_A;
            cpu.Flags = cpu.reg_F;
            cpu.reg_A = temp;
            cpu.reg_F = temp2;

            cpu.cycleCnt += 4;
            break;
        }    

        case 0xeb: //EXCHANGE INSTRUCTION - swap de with hl
        {
            int8_t temp = cpu.regD;
            int8_t temp2 = cpu.regE;
            cpu.regD = cpu.regH;
            cpu.regE = cpu.regL;
            cpu.regH = temp;
            cpu.regL = temp2;

            cpu.cycleCnt += 4;
            break;
        }

        case 0xd9: //EXCHANGE INSTRUCTION - swap bc, de,hl with b'c' d'e' h'l'
        {
            int8_t temp = cpu.regB;
            int8_t temp2 = cpu.regC;
            cpu.regB = cpu.reg_B;
            cpu.regC = cpu.reg_C;
            cpu.reg_B = temp;
            cpu.reg_C = temp2;

            temp = cpu.regD;
            temp2 = cpu.regE;
            cpu.regD = cpu.reg_D;
            cpu.regE = cpu.reg_E;
            cpu.reg_D = temp;
            cpu.reg_E = temp2;
        
            temp = cpu.regH;
            temp2 = cpu.regL;
            cpu.regH = cpu.reg_H;
            cpu.regL = cpu.reg_L;
            cpu.reg_H = temp;
            cpu.reg_L = temp2;

            cpu.cycleCnt += 4;
            break;
        }

        case  0xe3: //EXCHANGE INSTRUCTION - swap (sp) with L and (sp+1) with H
        {
            uint8_t temp = cpu.regH;
            uint8_t temp2 = cpu.regL;
            cpu.regL = z80_mem_read(cpu.reg_SP);
            cpu.regH = z80_mem_read(cpu.reg_SP+1);            
            z80_mem_write16(cpu.reg_SP, (temp << 8)|temp2);

            cpu.cycleCnt += 19;
            break;
        }


            

    }
        
        //LOAD INSTRUCTIONS=======================================================================================
    {
        //LOAD REGISTER FROM IMMEDIATE------------------------------------------------
        case 0x3e: //LOAD INSTRUCTION - Load value at n into register A
            //cpu.regA = memory[int(cpu.reg_PC++)]; //OLD WAY - WORKS BUT REPLACED WITH FUNCTION JUST INCASE
            cpu.regA = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x06: //LOAD INSTRUCTION - Load value at n into register B
            cpu.regB = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x0e: //LOAD INSTRUCTION - Load value at n into register C
            cpu.regC = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x16: //LOAD INSTRUCTION - Load value at n into register D
            cpu.regD = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x1e: //LOAD INSTRUCTION - Load value at n into register E
            cpu.regE = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x26: //LOAD INSTRUCTION - Load value at n into register H
            cpu.regH = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x2e: //LOAD INSTRUCTION - Load value at n into register L
            cpu.regL = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
        
        case 0x01: //LOAD INSTRUCTION - Load 16 bit value nn into paired register BC
            cpu.regB = z80_mem_read(int(cpu.reg_PC++));
            cpu.regC = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 10;
            break;
        
        case 0x11: //LOAD INSTRUCTION - Load 16 bit value nn into paired register DE
            cpu.regD = z80_mem_read(int(cpu.reg_PC++));
            cpu.regE = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 10;
            break;
        
        case 0x21: //LOAD INSTRUCTION - Load 16 bit value nn into paired register HL
            cpu.regH = z80_mem_read(int(cpu.reg_PC++));
            cpu.regL = z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 10;
            break;
        
        case 0x31: //LOAD INSTRUCTION - Load 16 bit value nn into paired register DE
            cpu.reg_SP = ((z80_mem_read(int(cpu.reg_PC++)) << 8) | z80_mem_read(int(cpu.reg_PC++)));
            cpu.cycleCnt += 10;
            break;

        //LOAD REGISTER FROM REGISTER INSTRUCTIONS------------------------------------------
        case 0x40: //LOAD INSTUCTION - Load Register B with Register B
            cpu.regB = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x41: //LOAD INSTUCTION - Load Register B with Register C
            cpu.regB = cpu.regC;
            cpu.cycleCnt += 4;
            break;

        case 0x42: //LOAD INSTUCTION - Load Register B with Register D
            cpu.regB = cpu.regD;
            cpu.cycleCnt += 4;
            break;
        
        case 0x43: //LOAD INSTUCTION - Load Register B with Register E
            cpu.regB = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x44: //LOAD INSTUCTION - Load Register B with Register H
            cout << "IN THE WRONG PLACE " << endl;
            cpu.regB = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x45: //LOAD INSTUCTION - Load Register B with Register L
            cpu.regB = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x47: //LOAD INSTUCTION - Load Register B with Register A
            cpu.regB = cpu.regA;
            cpu.cycleCnt += 4;
            break;

        case 0x48: //LOAD INSTUCTION - Load Register C with Register B
            cpu.regC = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x49: //LOAD INSTUCTION - Load Register C with Register C
            cpu.regC = cpu.regC;
            cpu.cycleCnt += 4;
            break;
        
        case 0x4A: //LOAD INSTUCTION - Load Register C with Register D
            cpu.regC = cpu.regD;
            cpu.cycleCnt += 4;
            break;

        case 0x4B: //LOAD INSTUCTION - Load Register C with Register E
            cpu.regC = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x4C: //LOAD INSTUCTION - Load Register C with Register H
            cpu.regC = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x4D: //LOAD INSTUCTION - Load Register C with Register L
            cpu.regC = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x4F: //LOAD INSTUCTION - Load Register C with Register A
            cpu.regC = cpu.regA;
            cpu.cycleCnt += 4;
            break;
        
        case 0x50: //LOAD INSTUCTION - Load Register D with Register B
            cpu.regD = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x51: //LOAD INSTUCTION - Load Register D with Register C
            cpu.regD = cpu.regC;
            cpu.cycleCnt += 4;
            break;

        case 0x52: //LOAD INSTUCTION - Load Register D with Register D
            cpu.regD = cpu.regD;
            cpu.cycleCnt += 4;
            break;
        
        case 0x53: //LOAD INSTUCTION - Load Register D with Register E
            cpu.regD = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x54: //LOAD INSTUCTION - Load Register D with Register H
            cpu.regD = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x55: //LOAD INSTUCTION - Load Register D with Register L
            cpu.regD = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x57: //LOAD INSTUCTION - Load Register D with Register A
            cpu.regD = cpu.regA;
            cpu.cycleCnt += 4;
            break;
        
        case 0x58: //LOAD INSTUCTION - Load Register E with Register B
            cpu.regE = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x59: //LOAD INSTUCTION - Load Register E with Register C
            cpu.regE = cpu.regC;
            cpu.cycleCnt += 4;
            break;
        
        case 0x5A: //LOAD INSTUCTION - Load Register E with Register D
            cpu.regE = cpu.regD;
            cpu.cycleCnt += 4;
            break;

        case 0x5B: //LOAD INSTUCTION - Load Register E with Register E
            cpu.regE = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x5C: //LOAD INSTUCTION - Load Register E with Register H
            cpu.regE = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x5D: //LOAD INSTUCTION - Load Register E with Register L
            cpu.regE = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x5F: //LOAD INSTUCTION - Load Register E with Register A
            cpu.regE = cpu.regA;
            cpu.cycleCnt += 4;
            break;
        
        case 0x60: //LOAD INSTUCTION - Load Register H with Register B
            cpu.regH = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x61: //LOAD INSTUCTION - Load Register H with Register C
            cpu.regH = cpu.regC;
            cpu.cycleCnt += 4;
            break;

        case 0x62: //LOAD INSTUCTION - Load Register H with Register D
            cpu.regH = cpu.regD;
            cpu.cycleCnt += 4;
            break;
        
        case 0x63: //LOAD INSTUCTION - Load Register H with Register E
            cpu.regH = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x64: //LOAD INSTUCTION - Load Register H with Register H
            cpu.regH = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x65: //LOAD INSTUCTION - Load Register H with Register L
            cpu.regH = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x67: //LOAD INSTUCTION - Load Register H with Register A
            cpu.regH = cpu.regA;
            cpu.cycleCnt += 4;
            break;

        case 0x68: //LOAD INSTUCTION - Load Register L with Register B
            cpu.regL = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x69: //LOAD INSTUCTION - Load Register L with Register C
            cpu.regL = cpu.regC;
            cpu.cycleCnt += 4;
            break;
        
        case 0x6A: //LOAD INSTUCTION - Load Register L with Register D
            cpu.regL = cpu.regD;
            cpu.cycleCnt += 4;
            break;

        case 0x6B: //LOAD INSTUCTION - Load Register L with Register E
            cpu.regL = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x6C: //LOAD INSTUCTION - Load Register L with Register H
            cpu.regL = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x6D: //LOAD INSTUCTION - Load Register L with Register L
            cpu.regL = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x6F: //LOAD INSTUCTION - Load Register L with Register A
            cpu.regL = cpu.regA;
            cpu.cycleCnt += 4;
            break;
        
        case 0x78: //LOAD INSTUCTION - Load Register A with Register B
            cpu.regA = cpu.regB;
            cpu.cycleCnt += 4;
            break;
        
        case 0x79: //LOAD INSTUCTION - Load Register A with Register C
            cpu.regA = cpu.regC;
            cpu.cycleCnt += 4;
            break;
        
        case 0x7A: //LOAD INSTUCTION - Load Register A with Register D
            cpu.regA = cpu.regD;
            cpu.cycleCnt += 4;
            break;

        case 0x7B: //LOAD INSTUCTION - Load Register A with Register E
            cpu.regA = cpu.regE;
            cpu.cycleCnt += 4;
            break;
        
        case 0x7C: //LOAD INSTUCTION - Load Register A with Register H
            cpu.regA = cpu.regH;
            cpu.cycleCnt += 4;
            break;
        
        case 0x7D: //LOAD INSTUCTION - Load Register A with Register L
            cpu.regA = cpu.regL;
            cpu.cycleCnt += 4;
            break;
        
        case 0x7F: //LOAD INSTUCTION - Load Register A with Register A
            cpu.regA = cpu.regA;
            cpu.cycleCnt += 4;
            break;

        case 0xf9: //LOAD INSTRUCTION - Load 16 bit Register SP with Paired Register HL
            cpu.reg_SP = ((cpu.regH << 8) | cpu.regL);
            cpu.cycleCnt += 6;
            break;
        
        //LOAD REGISTER FROM MEMORY ADDRESS-------------------------------------------------
        case 0x46: //LOAD INSTRUCTION - Load Register B with data from memory address stored in (HL)
            cpu.regB = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x4e: //LOAD INSTRUCTION - Load Register C with data from memory address stored in (HL)
            cpu.regC = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x56: //LOAD INSTRUCTION - Load Register D with data from memory address stored in (HL)
            cpu.regD = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x5e: //LOAD INSTRUCTION - Load Register E with data from memory address stored in (HL)
            cpu.regE = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x66: //LOAD INSTRUCTION - Load Register H with data from memory address stored in (HL)
            cpu.regH = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x6e: //LOAD INSTRUCTION - Load Register L with data from memory address stored in (HL)
            cpu.regL = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x7e: //LOAD INSTRUCTION - Load Register A with data from memory address stored in (HL)
            cpu.regA = z80_mem_read(int((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x0a: //LOAD INSTRUCTION - Load Register A with data from memory address stored in (BC)
            cpu.regA = z80_mem_read(int((((cpu.regB << 8) | cpu.regC))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x1a: //LOAD INSTRUCTION - Load Register A with data from memory address stored in (DE)
            cpu.regA = z80_mem_read(int((((cpu.regD << 8) | cpu.regE))));
            cpu.cycleCnt += 7;
            break;
        
        case 0x2a: //LOAD INSTRUCTION - Load Paired Register HL with data from memory address nn, and nn+1
        {    
            uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
            cpu.regL = z80_mem_read(addr); //NOTE THE NN GOES INTO L
            cpu.regH = z80_mem_read(addr + 1); //NOTE THE NN+1 GOES INTO H
            cpu.cycleCnt += 16;
            break;
        }

        case 0x3a: //LOAD INSTRUCTION - Load Register A with data from memory address stored in (nn)
            cpu.regA = z80_mem_read(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1)));
            cpu.cycleCnt += 13;
            break;
        
        //LOAD MEMORY ADDRESS FROM REGISTER-------------------------------------------------------------------
        case 0x70: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register B
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regB);
            cpu.cycleCnt += 7;
            break;
        
        case 0x71: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register C
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regC);
            cpu.cycleCnt += 7;
            break;
        
        case 0x72: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register D
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regD);
            cpu.cycleCnt += 7;
            break;
        
        case 0x73: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register E
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regE);
            cpu.cycleCnt += 7;
            break;
        
        case 0x74: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register H
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regH);
            cpu.cycleCnt += 7;
            break;
        
        case 0x75: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register L
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regL);
            cpu.cycleCnt += 7;
            break;
        
        case 0x77: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data from Register A
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), cpu.regA);
            cpu.cycleCnt += 7;
            break;
        
        case 0x02: //LOAD INSTRUCTION - Load Memory Address stored in (BC) with data from Register A
            z80_mem_write(int((((cpu.regB << 8) | cpu.regC))), cpu.regA);
            cpu.cycleCnt += 7;
            break;
        
        case 0x12: //LOAD INSTRUCTION - Load Memory Address stored in (DE) with data from Register A
            z80_mem_write(int((((cpu.regD << 8) | cpu.regE))), cpu.regA);
            cpu.cycleCnt += 7;
            break;
        
        case 0x32: //LOAD INSTRUCTION - Load Memory Address nn with data from register A
            z80_mem_write(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1)), cpu.regA);
            cpu.cycleCnt += 13;
            break;
        
        case 0x22: //LOAD INSTRUCTION - Load Memory Address nn with data from paired register HL
        {
            uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
            z80_mem_write(addr, cpu.regL); //L goes into (nn)
            z80_mem_write(addr+1, cpu.regH); //H goes into (nn+1)
            cpu.cycleCnt += 16;
            break;
        }
        
        //LOAD MEMORY ADDRESS FROM IMMEDIATE ----------------------------------------------------------------
        case 0x36: //LOAD INSTRUCTION - Load Memory Address stored in (HL) with data in next instruction
            z80_mem_write(int((((cpu.regH << 8) | cpu.regL))), z80_mem_read(int(cpu.reg_PC++)));
            cpu.cycleCnt += 10;
            break;
        

    }



        //ARITHMETIC INSTRUCTIONS==================================================================================
    {    
        //CARRY SET INSTRUCTIONS-------------------------------------------------------------
        case 0x37: //SET CARRY INSTRUCTION scf
            cpu.Flags |= 0x01; //Set the carry flag
            cpu.Flags &= ~0x02;//reset N
            cpu.Flags &= ~0x08;//reset H
            cpu.cycleCnt += 4;
            break;
        
        case 0x3F: //INVERT CARRY INSTRUCTION ccf - invert carry and half carry, reset N
            (cpu.Flags & 0x01) ? (cpu.Flags &= ~0x01) : (cpu.Flags |= 0x01); //invert carry
            cpu.Flags &= ~0x02;//reset N
            (cpu.Flags & 0x08) ? (cpu.Flags &= ~0x08) : (cpu.Flags |= 0x08); //invery half carry
            cpu.cycleCnt += 4;
            break;

        //ONES COMPLEMENT INSTRUCTION--------------------------------
        case 0x2F: //INVERSION OF A INSTRUCTION cpl - invert all bits in regA, sets H and N
            cpu.regA = ~cpu.regA; //invert bits
            cpu.Flags |= 0x02;//set N
            cpu.Flags |= 0x08;//set H
            cpu.cycleCnt += 4;
            break;

        //ADDITION INSTRUCTIONS---------------------------------------------------------------
        case 0x80: //ADD INSTRUCTION - RegA += RegB
            cpu.regA = addFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt += 4;
            break;

        case 0x81: //ADD INSTRUCTION - RegA += RegC
            cpu.regA = addFlags(cpu.regA, cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x82: //ADD INSTRUCTION - RegA += RegD
            cpu.regA = addFlags(cpu.regA, cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x83: //ADD INSTRUCTION - RegA += RegE 
            cpu.regA = addFlags(cpu.regA, cpu.regE);
            cpu.cycleCnt += 4;
            break;
        
        case 0x84: //ADD INSTRUCTION - RegA += RegH 
            cpu.regA = addFlags(cpu.regA, cpu.regH);
            cpu.cycleCnt += 4;
            break;
        
        case 0x85: //ADD INSTRUCTION - RegA += RegL 
            cpu.regA = addFlags(cpu.regA, cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x87: //ADD INSTRUCTION - RegA += RegA
            cpu.regA = addFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0xc6: //ADD IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = addFlags(cpu.regA,z80_mem_read(int(cpu.reg_PC++)));
            cpu.cycleCnt += 7;
            break;
        
        case 0x86: //ADD INSTRUCTION - RegA += memory[regHL]
            cpu.regA = addFlags(cpu.regA,z80_mem_read((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;


        case 0x09://PAIRED ADD INTRUCTION - Reg hl += bc
        {
            uint16_t paired = add16Flags(cpu.regH, cpu.regL, ((cpu.regB << 8)|cpu.regC));
            cpu.regH = (paired >> 8);   // Upper 8
            cpu.regL = (paired) & 0xFF; // Lower 8
            cpu.cycleCnt += 11;
            break;
        }    
    
        case 0x19: //PAIRED ADD INTRUCTION - Reg hl += de
        {
            uint16_t paired = add16Flags(cpu.regH, cpu.regL, ((cpu.regD << 8)|cpu.regE));
            cpu.regH = (paired >> 8);   // Upper 8
            cpu.regL = (paired) & 0xFF; // Lower 8
            cpu.cycleCnt += 11;
            break;
        } 

        case 0x29: //PAIRED ADD INTRUCTION - Reg hl += hl
        {
            uint16_t paired = add16Flags(cpu.regH, cpu.regL, ((cpu.regH << 8)|cpu.regL));
            cpu.regH = (paired >> 8);   // Upper 8
            cpu.regL = (paired) & 0xFF; // Lower 8
            cpu.cycleCnt += 11;
            break;
        } 
        
        case 0x39: //PAIRED ADD INTRUCTION - Reg hl += sp
        {
            uint16_t paired = add16Flags(cpu.regH, cpu.regL, cpu.reg_SP);
            cpu.regH = (paired >> 8);   // Upper 8
            cpu.regL = (paired) & 0xFF; // Lower 8
            cpu.cycleCnt += 11;
            break;
        }

        // INCREMENT INST -------------------------------------------------
        case 0x04: //INCRAMENT INSTRUCTION - Adds 1 to Register B
            cpu.regB = incFlags(cpu.regB);
            cpu.cycleCnt += 4;
            break;
        
        case 0x14: //INCRAMENT INSTRUCTION - Adds 1 to Register D
            cpu.regD = incFlags(cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x24: //INCRAMENT INSTRUCTION - Adds 1 to Register H
            cpu.regH = incFlags(cpu.regH);
            cpu.cycleCnt += 4;
            break;
        
        case 0x0c: //INCRAMENT INSTRUCTION - Adds 1 to Register C
            cpu.regC = incFlags(cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x1c: //INCRAMENT INSTRUCTION - Adds 1 to Register E
            cpu.regE = incFlags(cpu.regE);
            cpu.cycleCnt += 4;
            break;
        
        case 0x2c: //INCRAMENT INSTRUCTION - Adds 1 to Register L
            cpu.regL = incFlags(cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x3c: //INCRAMENT INSTRUCTION - Adds 1 to Register A
            cpu.regA = incFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0x03: //INCRAMENT PAIRED REGISTER INSTRUCTION - Adds 1 to Paired Register BC
        {
            uint16_t paired = (incPaired(cpu.regB, cpu.regC));
            cpu.regB = (paired >> 8) & 0xff;  // High byte
            cpu.regC = paired & 0xff;       // Low byte
            cpu.cycleCnt += 6;
            break;
        }
            
        
        case 0x13: //INCRAMENT PAIRED REGISTER INSTRUCTION - Adds 1 to Paired Register DE
        {
            uint16_t paired = (incPaired(cpu.regD, cpu.regE));
            cpu.regD = (paired >> 8) & 0xff;  // High byte
            cpu.regE = paired & 0xff;       // Low byte
            cpu.cycleCnt += 6;
            break;
        }
        
        case 0x23: //INCRAMENT PAIRED REGISTER INSTRUCTION - Adds 1 to Paired Register HL
        {
            uint16_t paired = (incPaired(cpu.regH, cpu.regL));
            cpu.regH = (paired >> 8) & 0xff;  // High byte
            cpu.regL = paired & 0xff;       // Low byte
            cpu.cycleCnt += 6;
            break;
        }
        
        case 0x33: //INCRAMENT 16bit REGISTER INSTRUCTION - Adds 1 to Paired Register SP
            cpu.reg_SP++; //Incrament by 1, do not set flags
            cpu.cycleCnt += 6;
            break;
        
        case 0x34: //INCRAMENT DATA AT MEMORY LOCATION INSTUCTION - Adds 1 to data at memory location (HL)
            z80_mem_write(((cpu.regH << 8) | cpu.regL), incFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 11;
            break;
        

        //ADD WITH CARRY INSTRUCTIONS---------------------------------------------------------------
        case 0x88: //ADD W CARRY INSTRUCTION - RegA += RegB + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt += 4;
            break;
        
        case 0x89: //ADD W CARRY INSTRUCTION - RegA += RegC + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8a: //ADD W CARRY INSTRUCTION - RegA += RegD + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8b: //ADD W CARRY INSTRUCTION - RegA += RegE + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regE);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8c: //ADD W CARRY INSTRUCTION - RegA += RegH + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regH);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8d: //ADD W CARRY INSTRUCTION - RegA += RegL + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8f: //ADD W CARRY INSTRUCTION - RegA += RegA + Carry
            cpu.regA = adcFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0x8e: //ADD W CARRY INSTRUCTION - RegA += (HL) + Carry
            cpu.regA = adcFlags(cpu.regA, z80_mem_read(((cpu.regH << 8) | cpu.regL)));
            cpu.cycleCnt += 7;
            break;
        
        case 0xce: //ADD W CARRY INSTRUCTION - RegA += n + Carry
            cpu.regA = adcFlags(cpu.regA, z80_mem_read(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;


        //SUBTRACTION ARITHMETIC-------------------------------------------------
        case 0x90: //SUBTRACTION INSTRUCTiON - RegA -= RegB
            cpu.regA = subFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt += 4;
            break;
        
        case 0x91: //SUBTRACTION INSTRUCTiON - RegA -= RegC
            cpu.regA = subFlags(cpu.regA, cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x92: //SUBTRACTION INSTRUCTiON - RegA -= RegD
            cpu.regA = subFlags(cpu.regA, cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x93: //SUBTRACTION INSTRUCTiON - RegA -= RegE
            cpu.regA = subFlags(cpu.regA, cpu.regE);
            cpu.cycleCnt += 4;
            break;

        case 0x94: //SUBTRACTION INSTRUCTiON - RegA -= RegH
            cpu.regA = subFlags(cpu.regA, cpu.regH);
            cpu.cycleCnt += 4;
            break;
        
        case 0x95: //SUBTRACTION INSTRUCTiON - RegA -= RegL
            cpu.regA = subFlags(cpu.regA, cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x97: //SUBTRACTION INSTRUCTiON - RegA -= RegA
            cpu.regA = subFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0xd6: //SUBTRACT IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = subFlags(cpu.regA,z80_mem_read(int(cpu.reg_PC++)));
            cpu.cycleCnt += 7;
            break;
        
        case 0x96: //SUBTRACTION INSTRUCTION - RegA -= memory[regHL]
            cpu.regA = subFlags(cpu.regA,z80_mem_read((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;

        //DEC INSTRUCTION---------------------------------------------------------------
        case 0x05: //DECREMENT INSTRUCTION - RegB -= 1, carry flag unaffected
            cpu.regB = decFlags(cpu.regB);
            cpu.cycleCnt += 4;
            break;
        
        case 0x15: //DECREMENT INSTRUCTION - RegD -= 1, carry flag unaffected
            cpu.regD = decFlags(cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x25: //DECREMENT INSTRUCTION - RegH -= 1, carry flag unaffected
            cpu.regH = decFlags(cpu.regH);
            cpu.cycleCnt += 4;
            break;

        case 0x0d: //DECREMENT INSTRUCTION - RegC -= 1, carry flag unaffected
            cpu.regC = decFlags(cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x1d: //DECREMENT INSTRUCTION - RegE -= 1, carry flag unaffected
            cpu.regE = decFlags(cpu.regE);
            cpu.cycleCnt += 4;
            break;
        
        case 0x2d: //DECREMENT INSTRUCTION - RegL -= 1, carry flag unaffected
            cpu.regL = decFlags(cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x3d: //DECREMENT INSTRUCTION - RegA -= 1, carry flag unaffected
            cpu.regA = decFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0x35:
            z80_mem_write(((cpu.regH << 8) | cpu.regL), decFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 11;
            break;
        
        case 0x0b: //PAIRED DECREMENT INSTRuCTION - Paired Reg BC - 1, no flags affected
        {
            uint16_t paired = decPaired(cpu.regB, cpu.regC);
            cpu.regB = (paired >> 8) & 0xff; //High
            cpu.regC = paired & 0xff; //Low
            cpu.cycleCnt += 6;
            break;
        }

        case 0x1b: //PAIRED DECREMENT INSTRuCTION - Paired Reg DE - 1, no flags affected
        {
            uint16_t paired = decPaired(cpu.regD, cpu.regE);
            cpu.regD = (paired >> 8) & 0xff; //High
            cpu.regE = paired & 0xff; //Low
            cpu.cycleCnt += 6;
            break;
        }

        case 0x2b: //PAIRED DECREMENT INSTRuCTION - Paired Reg HL - 1, no flags affected
        {
            uint16_t paired = decPaired(cpu.regH, cpu.regL);
            cpu.regH = (paired >> 8) & 0xff; //High
            cpu.regL = paired & 0xff; //Low
            cpu.cycleCnt += 6;
            break;
        }

        case 0x3b: //16 bit DECREMENT INSTRUCTION - RegSP--, no flags change
            cpu.reg_SP--;
            cpu.cycleCnt += 6;
            break;


        // SUBTRACT FLAGS CHANGE, RESGISTERS DONT-----------------------------------------------------
        case 0xb8: //cp B from A
            subFlags(cpu.reg_A, cpu.reg_B);
            cpu.cycleCnt+=4;
            break;

        case 0xb9: //cp C from A
            subFlags(cpu.reg_A, cpu.reg_C);
            cpu.cycleCnt+=4;
            break;

        case 0xba: //cp D from A
            subFlags(cpu.reg_A, cpu.reg_D);
            cpu.cycleCnt+=4;
            break;

        case 0xbb: //cp E from A
            subFlags(cpu.reg_A, cpu.reg_E);
            cpu.cycleCnt+=4;
            break; 

        case 0xbc: //cp H from A
            subFlags(cpu.reg_A, cpu.reg_H);
            cpu.cycleCnt+=4;
            break;

        case 0xbd: //cp L from A
            subFlags(cpu.reg_A, cpu.reg_L);
            cpu.cycleCnt+=4;
            break;

        case 0xbe: //cp HL from A
            subFlags(cpu.reg_A, z80_mem_read(((cpu.regH << 8) | cpu.regL)));
            cpu.cycleCnt+=7;
            break;

        case 0xbf: //cp A from A
            subFlags(cpu.reg_A, cpu.reg_A);
            cpu.cycleCnt+=4;
            break;
        
        case 0xfe: //cp n from A
            subFlags(cpu.reg_A, z80_mem_read(cpu.reg_PC++));
            cpu.cycleCnt+=7;
            break;


        //SUBTRACT WITH CARRY INSTRUCTIONS-------------------------------------------------------
        case 0x98:  //SUB W CARRY INSTRUCTION - RegA -= RegB - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt += 4;
            break;
        
        case 0x99:  //SUB W CARRY INSTRUCTION - RegA -= RegC - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regC);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9a:  //SUB W CARRY INSTRUCTION - RegA -= RegD - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regD);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9b:  //SUB W CARRY INSTRUCTION - RegA -= RegE - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regE);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9c:  //SUB W CARRY INSTRUCTION - RegA -= RegH - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regH);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9d:  //SUB W CARRY INSTRUCTION - RegA -= RegL - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regL);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9f:  //SUB W CARRY INSTRUCTION - RegA -= RegA - Carry
            cpu.regA = sbcFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0x9e:  //SUB W CARRY INSTRUCTION - RegA -= (hl) - Carry
            cpu.regA = sbcFlags(cpu.regA, z80_mem_read(((cpu.regH << 8) | cpu.regL)));
            cpu.cycleCnt += 7;
            break;
        
        case 0xde:  //SUB W CARRY INSTRUCTION - RegA -= n - Carry
            cpu.regA = sbcFlags(cpu.regA, z80_mem_read(cpu.reg_PC++));
            cpu.cycleCnt += 7;
            break;
   
    }

        //LOGICAL INSTRUCTIONS=====================================================================================
    { 

        // JUMP INST -----------------------------------------------------------------------------------------------------------------

        case 0x10: // jump if zero is not set after decrementing b reg 
            cpu.reg_B--;
            if(cpu.reg_B > 0)
            {
                cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt+=13;
                break;
            }
            cpu.cycleCnt+=8;
            break;



        case 0x20: // jump if zero is unset 
            if ((cpu.Flags & 0b010000000) == 0)
            {
                cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 12; 
                break;
            }
            cpu.cycleCnt+=7;
            break;


            
        case 0x30: // jump if carry is unset
        if ((cpu.Flags & 0b000000001) == 0)
            {
                cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 12; 
                break;
            }
            cpu.cycleCnt+=7; 
            break;



        case 0x18: // jump from inst opcode to d 
            cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 12; 
            break;



        case 0x28: // jump if zero flag is set from opcode to d 
            if ((cpu.Flags & 0b010000000) > 0)
            {
                cpu.reg_PC += memory[int(cpu.reg_PC++)];
                cpu.cycleCnt += 12; 
                break;
            }
            cpu.cycleCnt+=7;
            break;



        case 0x38: // jump is carry flag is set from opcode to d 
            if ((cpu.Flags & 0b000000001) > 0)
            {
                cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 12; 
                break;
            }
            cpu.cycleCnt+=7;
            break;


        case 0xc2: //zero flag unset, jump to nn
        if ((cpu.Flags & 0b010000000) == 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xd2:
        if ((cpu.Flags & 0b000000001) == 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xe2:
        if ((cpu.Flags & 0b000000100) == 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xf2:
        if ((cpu.Flags & 0b100000000) == 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;


        case 0xca: //zero flag set, jump to nn
        if ((cpu.Flags & 0b010000000) > 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xda: // if carry flag set jump to nn 
        if ((cpu.Flags & 0b000000001) > 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xea: // uf the P/V flag is unset, jump to nn
        if ((cpu.Flags & 0b000000100) > 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xfa: // if the sign flag us set, jump to nn
        if ((cpu.Flags & 0b100000000) > 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; 
                break;
            }
            cpu.cycleCnt+=10;
            break;


        case 0xc3: //jumps to nn if zero flag isnt set
            if ((cpu.Flags & 0b010000000) == 0)
            {
                cpu.reg_PC = z80_mem_read(int(cpu.reg_PC++));
                cpu.cycleCnt += 10; //cycle count for Jp   
                break;
            }
            break;




       













        case 0xcb: // if called, bit instructions 
        {

            cpu.reg_R++; //whatever is in paper - 1 (cause one already added prior)

            uint8_t inst2 = z80_mem_read(cpu.reg_PC++); //Reads the next instuction and incraments program counter

            switch(inst2)
            {

        //SHIFT INSTRUCNTIONS-------------------------------------------------------------------------

        //memwrite(emma code for z80 read, bit to manipulate)


                    case 0x00: // RLC b 
                        ((cpu.regB & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB = (cpu.regB << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x01: // RLC c
                        ((cpu.regC & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC = (cpu.regC << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x02: // RLC d
                        ((cpu.regD & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD = (cpu.regD << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x03: // RLC e
                        ((cpu.regE & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE = (cpu.regE << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x04: // RLC h
                        ((cpu.regH & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH = (cpu.regH << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x05: // RLC l
                        ((cpu.regL & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL = (cpu.regL << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;

                    case 0x06: // RLC (hl)
                        ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), (z80_mem_read(((cpu.regH << 8) | cpu.regL)) << 1) | (cpu.Flags & 0x01));
                        cpu.Flags = ZSPXYtable[int(z80_mem_read(((cpu.regH << 8) | cpu.regL)))]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=15;
                        break;

                    case 0x07: // RLC a
                        ((cpu.regA & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA = (cpu.regA << 1) | (cpu.Flags & 0x01);
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=8;
                        break;







                    case 0x08: //RRC b
                        ((cpu.regB & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB = (cpu.regB >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x09: //RRC c
                        ((cpu.regC & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC = (cpu.regC >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0A: //RRC d
                        ((cpu.regD & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD = (cpu.regD >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0B: //RRC e
                        ((cpu.regE & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE = (cpu.regE >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0C: //RRC h
                        ((cpu.regH & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH = (cpu.regH >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0D: //RRC l
                        ((cpu.regL & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL = (cpu.regL >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0E: //RRC (hl)
                        ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1) | (cpu.Flags & 0x01)));
                        cpu.Flags = ZSPXYtable[int(z80_mem_read(((cpu.regH << 8) | cpu.regL)))]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=15;
                        break;

                    case 0x0F: //RRC a
                        ((cpu.regA & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA = (cpu.regA >> 1) | (cpu.Flags & 0x80);
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;






                    case 0x10: // rl b

                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regB & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB = (cpu.regB << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x11: // rl c

                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regC & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC = (cpu.regC << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x12: // rl d
                    
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regD & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD = (cpu.regD << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x13: // rl e
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regE & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE = (cpu.regE << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }
                        
                        break; 

                    case 0x14: // rl h
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regH & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH = (cpu.regH << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }
                        
                        break;

                    case 0x15: // rl l
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regL & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL = (cpu.regL << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }
                        
                        break;

                    case 0x16: // rl (hl)
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ( (z80_mem_read(((cpu.regH << 8) | cpu.regL)))| preFlagC));
                        cpu.Flags = ZSPXYtable[int(z80_mem_read(((cpu.regH << 8) | cpu.regL)))]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=15;
                        }
                        
                        break;

                    case 0x17: // rl a
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regA & 0x80)==0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA = (cpu.regA << 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }
                        
                        break;









                    case 0x18: // rr b 

                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regB & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB = (cpu.regB >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x19: // rr c
                    
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regC & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC = (cpu.regC >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1A: // rr d
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regD & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD = (cpu.regD >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1B: // rr e
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regE & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE = (cpu.regE >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1C: // rr h
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regH & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH = (cpu.regH >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1D: // rr l
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regL & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL = (cpu.regL >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1E: // rr (hl)
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1)| preFlagC));
                        cpu.Flags = ZSPXYtable[int(z80_mem_read(((cpu.regH << 8) | cpu.regL)))]; //Set Z, S, X, Y 
                        cpu.cycleCnt+=15;
                        }
                        
                        break;

                    case 0x1F: // rr a
                                        
                        {
                        uint8_t preFlagC = cpu.Flags & 0x01;
                        ((cpu.regA & 0x01)==0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA = (cpu.regA >> 1) | preFlagC;
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        }

                        break;







                    case 0x20: //sla b
                        ((cpu.regB & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x21: //sla c
                        ((cpu.regC & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x22: //sla d
                        ((cpu.regD & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x23: //sla e
                        ((cpu.regE & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x24: //sla h
                        ((cpu.regH & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x25: //sla l
                        ((cpu.regL & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL <<= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x26: //sla (hl)
                       (((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01));
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) << 1)));
                        cpu.Flags = ZSPXYtable[int((z80_mem_read(((cpu.regH << 8) | cpu.regL))))]; //Set Z, S, X, Y
                        cpu.cycleCnt+=15;
                        break; 

                    case 0x27: //sla a
                    cout << int(cpu.regA) << ":" << int(cpu.regA << 1) << endl;
                        ((cpu.regA & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA = (cpu.regA << 1);
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;






                    case 0x28: // sra b
                        ((cpu.regB & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x29: // sra c
                        ((cpu.regC & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2A: // sra d
                        ((cpu.regD & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2B: // sra e
                        ((cpu.regE & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2C: // sra h
                        ((cpu.regH & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2D: // sra l
                        ((cpu.regL & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2E: // sra (hl)
                        (((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01));
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1)));
                        cpu.Flags = ZSPXYtable[int((z80_mem_read(((cpu.regH << 8) | cpu.regL))))]; //Set Z, S, X, Y
                        cpu.cycleCnt+=15;
                        break;
 
                    case 0x2F: // sra a
                        ((cpu.regA & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA >>= 1;
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;







                    case 0x30: // sll b 
                        ((cpu.regB & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB <<= 1;
                        cpu.regB |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x31: // sll c
                        ((cpu.regC & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC <<= 1;
                        cpu.regC |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x32: // sll d 
                        ((cpu.regD & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD <<= 1;
                        cpu.regD |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x33: // sll e 
                        ((cpu.regE & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE <<= 1;
                        cpu.regE |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x34: // sll h 
                        ((cpu.regH & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH <<= 1;
                        cpu.regH |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x35: // sll l 
                        ((cpu.regL & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL <<= 1;
                        cpu.regL |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                         
                    case 0x36: // sll (hl) 
                       (((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01));
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) << 1) | 0x01));
                        cpu.Flags = ZSPXYtable[int((z80_mem_read(((cpu.regH << 8) | cpu.regL))))]; //Set Z, S, X, Y
                        cpu.cycleCnt+=15;
                        break;
                        
                    case 0x37: // sll a
                        ((cpu.regA & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA <<= 1;
                        cpu.regB |= 0x01;
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;



                        
                    case 0x38: // srl b
                        ((cpu.regB & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regB >>= 1;
                        cpu.regB |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regB)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x39: // srl c
                        ((cpu.regC & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regC >>= 1;
                        cpu.regC |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regC)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3A: // srl d
                        ((cpu.regD & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regD >>= 1;
                        cpu.regD |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regD)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3B: // srl e
                        ((cpu.regE & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regE >>= 1;
                        cpu.regE |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regE)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3C: // srl h
                        ((cpu.regH & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regH >>= 1;
                        cpu.regH |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regH)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3D: // srl l
                        ((cpu.regL & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regL >>= 1;
                        cpu.regL |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regL)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3E: // srl (hl)
                        (((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01));
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1) | 0x80));
                        cpu.Flags = ZSPXYtable[int((z80_mem_read(((cpu.regH << 8) | cpu.regL))))]; //Set Z, S, X, Y
                        cpu.cycleCnt+=15;
                        break;
                        
                    case 0x3F: // srl a
                        ((cpu.regA & 0x01) == 0x01) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
                        cpu.regA >>= 1;
                        cpu.regA |= 0x80;
                        cpu.Flags = ZSPXYtable[int(cpu.regA)]; //Set Z, S, X, Y
                        cpu.cycleCnt+=8;
                        break;
                        



                    case 0x40:
                        break;
                    case 0x41:
                        break;
                    case 0x42:
                        break;
                    case 0x43:
                        break;
                    case 0x44:
                        break;
                    case 0x45:
                        break;
                    case 0x46:
                        break;
                    case 0x47:
                        break;
                    case 0x48:
                        break;
                    case 0x49:
                        break;
                    case 0x4A:
                        break;
                    case 0x4B:
                        break;
                    case 0x4C:
                        break;
                    case 0x4D:
                        break;
                    case 0x4E:
                        break;
                    case 0x4F:
                        break;
                    case 0x50:
                        break;
                    case 0x51:
                        break;
                    case 0x52:
                        break;
                    case 0x53:
                        break;
                    case 0x54:
                        break;
                    case 0x55:
                        break;
                    case 0x56:
                        break;
                    case 0x57:
                        break;
                    case 0x58:
                        break;
                    case 0x59:
                        break;
                    case 0x5A:
                        break;
                    case 0x5B:
                        break;
                    case 0x5C:
                        break;
                    case 0x5D:
                        break;
                    case 0x5E:
                        break;
                    case 0x5F:
                        break;
                    case 0x60:
                        break;
                    case 0x61:
                        break;
                    case 0x62:
                        break;
                    case 0x63:
                        break;
                    case 0x64:
                        break;
                    case 0x65:
                        break;
                    case 0x66:
                        break;
                    case 0x67:
                        break;
                    case 0x68:
                        break;
                    case 0x69:
                        break;
                    case 0x6A:
                        break;
                    case 0x6B:
                        break;
                    case 0x6C:
                        break;
                    case 0x6D:
                        break;
                    case 0x6E:
                        break;
                    case 0x6F:
                        break;
                    case 0x70:
                        break;
                    case 0x71:
                        break;
                    case 0x72:
                        break;
                    case 0x73:
                        break;
                    case 0x74:
                        break;
                    case 0x75:
                        break;
                    case 0x76:
                        break;
                    case 0x77:
                        break;
                    case 0x78:
                        break;
                    case 0x79:
                        break;
                    case 0x7A:
                        break;
                    case 0x7B:
                        break;
                    case 0x7C:
                        break;
                    case 0x7D:
                        break;
                    case 0x7E:
                        break;
                    case 0x7F:
                        break;
                    case 0x80:
                        break;
                    case 0x81:
                        break;
                    case 0x82:
                        break;
                    case 0x83:
                        break;
                    case 0x84:
                        break;
                    case 0x85:
                        break;
                    case 0x86:
                        break;
                    case 0x87:
                        break;
                    case 0x88:
                        break;
                    case 0x89:
                        break;
                    case 0x8A:
                        break;
                    case 0x8B:
                        break;
                    case 0x8C:
                        break;
                    case 0x8D:
                        break;
                    case 0x8E:
                        break;
                    case 0x8F:
                        break;
                    case 0x90:
                        break;
                    case 0x91:
                        break;
                    case 0x92:
                        break;
                    case 0x93:
                        break;
                    case 0x94:
                        break;
                    case 0x95:
                        break;
                    case 0x96:
                        break;
                    case 0x97:
                        break;
                    case 0x98:
                        break;
                    case 0x99:
                        break;
                    case 0x9A:
                        break;
                    case 0x9B:
                        break;
                    case 0x9C:
                        break;
                    case 0x9D:
                        break;
                    case 0x9E:
                        break;
                    case 0x9F:
                        break;
                    case 0xA0:
                        break;
                    case 0xA1:
                        break;
                    case 0xA2:
                        break;
                    case 0xA3:
                        break;
                    case 0xA4:
                        break;
                    case 0xA5:
                        break;
                    case 0xA6:
                        break;
                    case 0xA7:
                        break;
                    case 0xA8:
                        break;
                    case 0xA9:
                        break;
                    case 0xAA:
                        break;
                    case 0xAB:
                        break;
                    case 0xAC:
                        break;
                    case 0xAD:
                        break;
                    case 0xAE:
                        break;
                    case 0xAF:
                        break;
                    case 0xB0:
                        break;
                    case 0xB1:
                        break;
                    case 0xB2:
                        break;
                    case 0xB3:
                        break;
                    case 0xB4:
                        break;
                    case 0xB5:
                        break;
                    case 0xB6:
                        break;
                    case 0xB7:
                        break;
                    case 0xB8:
                        break;
                    case 0xB9:
                        break;
                    case 0xBA:
                        break;
                    case 0xBB:
                        break;
                    case 0xBC:
                        break;
                    case 0xBD:
                        break;
                    case 0xBE:
                        break;
                    case 0xBF:
                        break;
                    case 0xC0:
                        break;
                    case 0xC1:
                        break;
                    case 0xC2:
                        break;
                    case 0xC3:
                        break;
                    case 0xC4:
                        break;
                    case 0xC5:
                        break;
                    case 0xC6:
                        break;
                    case 0xC7:
                        break;
                    case 0xC8:
                        break;
                    case 0xC9:
                        break;
                    case 0xCA:
                        break;
                    case 0xCB:
                        break;
                    case 0xCC:
                        break;
                    case 0xCD:
                        break;
                    case 0xCE:
                        break;
                    case 0xCF:
                        break;
                    case 0xD0:
                        break;
                    case 0xD1:
                        break;
                    case 0xD2:
                        break;
                    case 0xD3:
                        break;
                    case 0xD4:
                        break;
                    case 0xD5:
                        break;
                    case 0xD6:
                        break;
                    case 0xD7:
                        break;
                    case 0xD8:
                        break;
                    case 0xD9:
                        break;
                    case 0xDA:
                        break;
                    case 0xDB:
                        break;
                    case 0xDC:
                        break;
                    case 0xDD:
                        break;
                    case 0xDE:
                        break;
                    case 0xDF:
                        break;
                    case 0xE0:
                        break;
                    case 0xE1:
                        break;
                    case 0xE2:
                        break;
                    case 0xE3:
                        break;
                    case 0xE4:
                        break;
                    case 0xE5:
                        break;
                    case 0xE6:
                        break;
                    case 0xE7:
                        break;
                    case 0xE8:
                        break;
                    case 0xE9:
                        break;
                    case 0xEA:
                        break;
                    case 0xEB:
                        break;
                    case 0xEC:
                        break;
                    case 0xED:
                        break;
                    case 0xEE:
                        break;
                    case 0xEF:
                        break;
                    case 0xF0:
                        break;
                    case 0xF1:
                        break;
                    case 0xF2:
                        break;
                    case 0xF3:
                        break;
                    case 0xF4:
                        break;
                    case 0xF5:
                        break;
                    case 0xF6:
                        break;
                    case 0xF7:
                        break;
                    case 0xF8:
                        break;
                    case 0xF9:
                        break;
                    case 0xFA:
                        break;
                    case 0xFB:
                        break;
                    case 0xFC:
                        break;
                    case 0xFD:
                        break;
                    case 0xFE:
                        break;
                    case 0xFF:
                        break;


                }
                break;
            }








        



        //AND INSTRUCTIONS -----------------------------------------------
        case 0xa0: //bitwise and register A with B
            cpu.reg_A = cpu.reg_A & cpu.reg_B; //bitwise AND for reg a with b 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa1: //bitwise and register A with C
            cpu.reg_A = cpu.reg_A & cpu.reg_C; //bitwise AND for reg a with c
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa2: //bitwise and register A with D
            cpu.reg_A = cpu.reg_A & cpu.reg_D; //bitwise AND for reg a with d 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa3: //bitwise and register A with E
            cpu.reg_A = cpu.reg_A & cpu.reg_E; //bitwise AND for reg a with e 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa4: //bitwise and register A with H
            cpu.reg_A = cpu.reg_A & cpu.reg_H; //bitwise AND for reg a with H 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa5: //bitwise and register A with L
            cpu.reg_A = cpu.reg_A & cpu.reg_B; //bitwise AND for reg a with l 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa6: //bitwise and register A with HL
            cpu.reg_A = cpu.reg_A & z80_mem_read(((cpu.regH << 8) | cpu.regL)); //bitwise AND for reg a with HL 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;

        case 0xa7: //bitwise and register A with A
            cpu.reg_A = cpu.reg_A & cpu.reg_A; //bitwise AND for reg a with a 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xe6:
            cpu.reg_A = cpu.reg_A & memory[int(cpu.reg_PC++)]; //bitwise AND for reg a with n 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;





        //XOR Instruction --------------------------------------------------------
        case 0xa8: // XOR A with B 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_B; //bitwise XOR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa9: // XOR A with C 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_C; //bitwise XOR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xaa: // XOR A with D 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_D; //bitwise XOR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xab: // XOR A with E 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_E; //bitwise XOR for reg a with e  
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xac: // XOR A with H 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_H; //bitwise XOR for reg a with h 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xad: // XOR A with L 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_L; //bitwise XOR for reg a with l 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xae: // XOR A with HL 
            cpu.reg_A = cpu.reg_A ^ z80_mem_read(((cpu.regH << 8) | cpu.regL)); //bitwise XOR for reg a with hl 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;

        case 0xaf: // XOR A with A 
            cpu.reg_A = cpu.reg_A ^ cpu.reg_B; //bitwise XOR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xee:
            cpu.reg_A = cpu.reg_A ^ z80_mem_read(int(cpu.reg_PC++));
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;

            //im sorry, i swear this didn't take me long 

                          //////      ////////
                        //    ////    //      //
                        //   // //    //      //    ===========|
                        //  //  //    ////////                ||
                        ////    //    //      //           \  ||  /
                          //////      //      //            \ || /
//                                                            \ /

        //OR INSTRUCTIONS =================================================================
        case 0xb0: // OR A with B 
            cpu.reg_A = cpu.reg_A | cpu.reg_B; //bitwise OR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb1: // OR A with C 
            cpu.reg_A = cpu.reg_A | cpu.reg_C; //bitwise OR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb2: // OR A with D 
            cpu.reg_A = cpu.reg_A | cpu.reg_D; //bitwise OR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb3: // OR A with E 
            cpu.reg_A = cpu.reg_A | cpu.reg_E; //bitwise OR for reg a with e  
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb4: // OR A with H 
            cpu.reg_A = cpu.reg_A | cpu.reg_H; //bitwise OR for reg a with h 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb5: // OR A with L 
            cpu.reg_A = cpu.reg_A | cpu.reg_L; //bitwise OR for reg a with l 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb6: // OR A with HL 
            cpu.reg_A = cpu.reg_A | z80_mem_read(((cpu.regH << 8) | cpu.regL)); //bitwise OR for reg a with hl 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;

        case 0xb7: // OR A with A 
            cpu.reg_A = cpu.reg_A | cpu.reg_B; //bitwise OR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;
        
        case 0xf6:// OR A with n memory[int(cpu.reg_PC++)];
            cpu.reg_A = cpu.reg_A | memory[int(cpu.reg_PC++)]; //bitwise OR for reg a with a             
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 7;
            break;






        




        
        // PUSH FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------

        /*SP is decremented and B is stored into the memory location pointed to by SP.
         SP is decremented again and C is stored into the memory location pointed to by SP*/
        case 0xc5:
            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_B;

            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_C;

            cpu.cycleCnt += 11;
            
            break;

        /*SP is decremented and D is stored into the memory location pointed to by SP. 
        SP is decremented again and E is stored into the memory location pointed to by SP.*/
        case 0xd5: 
            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_D;

            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_E;

            cpu.cycleCnt += 11;
            
            break;

        /*SP is decremented and H is stored into the memory location pointed to by SP. 
        SP is decremented again and L is stored into the memory location pointed to by SP.*/
        case 0xe5:
            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_H;

            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_L;

            cpu.cycleCnt += 11;
            
            break;

        /*SP is decremented and A is stored into the memory location pointed to by SP. 
        SP is decremented again and F is stored into the memory location pointed to by SP.*/
        case 0xf5:
            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_A;

            cpu.reg_SP--;
            memory[cpu.reg_SP] = cpu.reg_F;

            cpu.cycleCnt += 11;
            
            break;
        
        // CALL FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 0xc4:
        //If the zero flag is unset
            if ( (cpu.Flags & 0b010000000) == 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xd4:
        //If the carry flag is unset
            if ( (cpu.Flags & 0b00000001) == 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xe4:
        //If the P/V flag is unset
            if ( (cpu.Flags & 0b00000100) == 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xf4:
        //If the sign flag is unset
            if ( (cpu.Flags & 0b10000000) == 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xcc:
        //If the zero flag is set
            if ( (cpu.Flags & 0b010000000) > 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xdc:
        //If the carry flag is set
            if ( (cpu.Flags & 0b00000001) > 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xec:
        //If the P/V flag is set
            if ( (cpu.Flags & 0b00000100) > 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xfc:
        //If the sign flag is set
            if ( (cpu.Flags & 0b10000000) > 0)
            {
                cpu.reg_SP = cpu.reg_PC + 3;
                cpu.reg_PC = memory[cpu.reg_PC++];
                cpu.cycleCnt+=17;
                break;
            }
            cpu.cycleCnt+=10;
            break;

        case 0xe9: // jp (hl): adds contence of HL into PC
            cpu.reg_PC = z80_mem_read(((cpu.regH << 8) | cpu.regL));
            cpu.cycleCnt+=4;
            break;


        case 0xcd:
        //unconditional jump call
            cpu.reg_SP = cpu.reg_PC + 3;
            cpu.reg_PC = memory[cpu.reg_PC++];
            cpu.cycleCnt+=17;
            break;

        
        
        // POP FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
    {
        case 0xc1: //POP into BC
        {
            uint16_t temp = popS();
            cpu.regB = temp >> 8;
            cpu.regC = temp & 0xff;
            cpu.cycleCnt += 10;
            break;
        }    

        case 0xd1: //POP into DE
        {
            uint16_t temp = popS();
            cpu.regD = temp >> 8;
            cpu.regE = temp & 0xff;
            cpu.cycleCnt += 10;
            break;
        }  

        case 0xe1: //POP into HL
        {
            uint16_t temp = popS();
            cpu.regH = temp >> 8;
            cpu.regL = temp & 0xff;
            cpu.cycleCnt += 10;
            break;
        }  

        case 0xf1: //POP into AF
        {
            uint16_t temp = popS();
            cpu.regA = temp >> 8;
            cpu.Flags = temp & 0xff;
            cpu.cycleCnt += 10;
            break;
        }  
    }







        // RETURN FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 0xc0: //zero flag unset: top stack entry is popped into PC.
            if ( (cpu.Flags & 0x40) == 0)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;
        
        case 0xc8: //zero flag set: top stack entry is popped into PC.
            if ( (cpu.Flags & 0x40) >> 6)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xd0: // carry flag unset
            if ( (cpu.Flags & 0x01) == 0)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;
        
        case 0xd8: // carry flag set
            if ( (cpu.Flags & 0x01))
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xe0: // P/V flag unset
            if ( (cpu.Flags & 0x04) == 0)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;
        
        case 0xe8: // P/V flag set
            if ( (cpu.Flags & 0x04) >> 2)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xf0: // sign flag unset
            if ( (cpu.Flags & 0x80) == 0)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;
        
        case 0xf8: // sign flag set
            if ( (cpu.Flags & 0x80) >>7)
            {
                retS();
                cpu.cycleCnt+=11;
                break;
            }
            cpu.cycleCnt+=5;
            break;
        
        case 0xc9: // ret
            retS();
            cpu.cycleCnt+=10;
            break;
            

        





        // INTERUPT HANDILING -------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 0xf3:
            
            INTERUPT_MODE1 = false;
            INTERUPT_MODE2 = false;
            
            cpu.cycleCnt+=4;

            break;

        case 0xfb:
            
            INTERUPT_MODE1 = true;
            INTERUPT_MODE2 = true;
            
            cpu.cycleCnt+=4;

            break;
        
        //================================================================================================================================
        //ED INSTRUCTIONS=================================================================================================================
        //================================================================================================================================
    {
        case 0xed:
            cpu.reg_R += 1; //ED increases R by 2, one is already done by while loop
            uint8_t inst2 = z80_mem_read(cpu.reg_PC++); //Reads the next instuction and incraments program counter
            switch(inst2)
            {
                //NOPS---------------------------------------------------------------------------------------
            {    
                case 0x40: break;
                case 0x50: break;
                case 0x60: break;
                case 0x70: break;

                case 0x41: break;
                case 0x51: break;
                case 0x61: break;
                case 0x71: break;

                case 0x48: break;
                case 0x58: break;
                case 0x68: break;
                case 0x78: break;

                case 0x49: break;
                case 0x59: break;
                case 0x69: break;
                case 0x79: break;

                case 0xa2: break;
                case 0xb2: break;
                case 0xa3: break;
                case 0xb3: break;
                case 0xaa: break;
                case 0xba: break;
                case 0xab: break;
                case 0xbb: break;
            }    
                
                //PROJECT SPECIFIC INSTRUCTIONS---------------------------------------------------------------
            {
                case 0x00: //Print out the state of all registers to the terminal
                    printReg(cpu);
                    cpu.cycleCnt += 0; //NOT SPECIFIED ON WEBPAGE SO I JUST WILL NOT CHANGE IT
                    break;
                
                case 0x01: //dump memory to a file called memory.bin
                    z80_mem_dump("memory.bin");
                    cpu.cycleCnt += 0; //NOT SPECIFIED ON WEBPAGE SO I JUST WILL NOT CHANGE IT
                    break;
            }

                //LOAD INSTRUCTIONS------------------------------------------------------------------------
            {
                case 0x43://LOAD INSTRUCTION - load memory address (nn) with C and (nn+1) with B 
                {    
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    z80_mem_write(addr, cpu.regC); //C goes into (nn)
                    z80_mem_write(addr+1, cpu.regB); //B goes into (nn+1)
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x53://LOAD INSTRUCTION - load memory address (nn) with E and (nn+1) with D 
                {    
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    z80_mem_write(addr, cpu.regE); //E goes into (nn)
                    z80_mem_write(addr+1, cpu.regD); //D goes into (nn+1)
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x63://LOAD INSTRUCTION - load memory address (nn) with L and (nn+1) with H 
                {    
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    z80_mem_write(addr, cpu.regL); //L goes into (nn)
                    z80_mem_write(addr+1, cpu.regH); //H goes into (nn+1)
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x73://LOAD INSTRUCTION - load memory address (nn) with sp LOWER8 and (nn+1) with sp HIGHER8
                {    
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    z80_mem_write(addr, cpu.reg_SP & 0xff ); //Lower8 goes into (nn)
                    z80_mem_write(addr+1, cpu.reg_SP >> 8 ); //higher 8 goes into (nn+1)
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x4b://LOAD INSTRUCTION - load value at (nn) into C and (nn+1) into B
                {
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    cpu.regC = z80_mem_read(addr); //NOTE THE NN GOES INTO C
                    cpu.regB = z80_mem_read(addr + 1); //NOTE THE NN+1 GOES INTO B
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x5b://LOAD INSTRUCTION - load value at (nn) into E and (nn+1) into D
                {
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    cpu.regE = z80_mem_read(addr); //NOTE THE NN GOES INTO E
                    cpu.regD = z80_mem_read(addr + 1); //NOTE THE NN+1 GOES INTO D
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x6b://LOAD INSTRUCTION - load value at (nn) into L and (nn+1) into H
                {
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    cpu.regL = z80_mem_read(addr); //NOTE THE NN GOES INTO L
                    cpu.regH = z80_mem_read(addr + 1); //NOTE THE NN+1 GOES INTO H
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x7b://LOAD INSTRUCTION - load value at (nn) into spLOWER8 and (nn+1) into spUPPER8
                {
                    uint16_t addr = ((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1));
                    cpu.reg_SP = (z80_mem_read(addr +1) << 8) | z80_mem_read(addr);
                    cpu.cycleCnt += 20;
                    break;
                }

                case 0x47://LOAD INSTRUCTION - load value of a into i
                    cpu.reg_I = cpu.regA;
                    cpu.cycleCnt += 9;
                    break;
                
                case 0x57://LOAD INSTRUCTION - load value of i into a, CHANGE FLAGS
                    cpu.regA = cpu.reg_I;

                    cpu.Flags = ZSXYtable[cpu.reg_I]|(cpu.Flags & 0x01);//S Z X Y, based on i, C unaffected
                    //H is reset.
                    cpu.Flags |= cpu.regIFF2 << 2;//P/V contains contents of IFF2, if interrupt durring instruction: reset
                    //N is reset.

                    cpu.cycleCnt += 9;
                    break;
                
                case 0x4f://LOAD INSTRUCTION - load value of a into r
                    cpu.reg_R = cpu.regA;
                    cpu.cycleCnt += 9;
                    break;
                
                case 0x5f://LOAD INSTRUCTION - load value of r into a, CHANGE FLAGS
                    cpu.regA = cpu.reg_R;

                    cpu.Flags = ZSXYtable[cpu.reg_R]|(cpu.Flags & 0x01);//S Z X Y, based on i, C unaffected
                    //H is reset.
                    cpu.Flags |= cpu.regIFF2 << 2;//P/V contains contents of IFF2, if interrupt durring instruction: reset
                    //N is reset.

                    cpu.cycleCnt += 9;
                    break;
            }
                
                //BLOCK TRANSFER INSTRUCTIONS------------------------------------------------------------------
            {
                case 0xa0://BLOCK TRANSF INSTRUCTION - (DE) ← (HL), DE ← DE + 1, HL ← HL + 1, BC ← BC – 1
                {   
                    uint8_t n = cpu.regA + z80_mem_read((cpu.regH << 8) | cpu.regL);

                    z80_mem_write((cpu.regD<<8)|cpu.regE, z80_mem_read((cpu.regH << 8) | cpu.regL)); //(DE) <- (HL)
                    cpu.regD = (((cpu.regD<<8)|cpu.regE) + 1) >> 8;//De++
                    cpu.regE = (((cpu.regD<<8)|cpu.regE) + 1) & 0xff;//dE++
                    cpu.regH = (((cpu.regH<<8)|cpu.regL) + 1) >> 8;//Hl++
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) + 1) & 0xff;//hL++
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--
                    
                    //S, Z, C are not affected.
                    cpu.Flags &= ~0x10; //H is reset.
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags &= ~0x02; //N is reset.
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a+(hl old) bit 1
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a+(hl old) bit 3

                    cpu.cycleCnt += 16;
                    break;
                }
                
                case 0xb0://BLOCK TRANSF INSTRUCTION - while (BC ≠ 0) {(DE) ← (HL), DE ← DE + 1, HL ← HL + 1, BC ← BC – 1}
                {    
                    while(bool(cpu.regB|cpu.regC)) //While BC != 0
                    {
                        uint8_t n = cpu.regA + z80_mem_read((cpu.regH << 8) | cpu.regL);

                        z80_mem_write((cpu.regD<<8)|cpu.regE, z80_mem_read((cpu.regH << 8) | cpu.regL)); //(DE) <- (HL)
                        cpu.regD = (((cpu.regD<<8)|cpu.regE) + 1) >> 8;//De++
                        cpu.regE = (((cpu.regD<<8)|cpu.regE) + 1) & 0xff;//dE++
                        cpu.regH = (((cpu.regH<<8)|cpu.regL) + 1) >> 8;//Hl++
                        cpu.regL = (((cpu.regH<<8)|cpu.regL) + 1) & 0xff;//hL++
                        cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                        cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--
                        
                        //S, Z, C are not affected.
                        cpu.Flags &= ~0x10; //H is reset.
                        (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                        cpu.Flags &= ~0x02; //N is reset.
                        (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a+(hl old) bit 1
                        (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a+(hl old) bit 3

                        cpu.reg_PC -= 2;
                        cpu.cycleCnt += 21;
                    }

                    cpu.cycleCnt += 16; //For BC = 0 check
                    break;
                }
                    
                case 0xa8://BLOCK TRANSF INSTRUCTION - (DE) ← (HL), DE ← DE – 1, HL ← HL– 1, BC ← BC– 1
                {
                    uint8_t n = cpu.regA + z80_mem_read((cpu.regH << 8) | cpu.regL);

                    z80_mem_write((cpu.regD<<8)|cpu.regE, z80_mem_read((cpu.regH << 8) | cpu.regL)); //(DE) <- (HL)
                    cpu.regD = (((cpu.regD<<8)|cpu.regE) - 1) >> 8;//De--
                    cpu.regE = (((cpu.regD<<8)|cpu.regE) - 1) & 0xff;//dE--
                    cpu.regH = (((cpu.regH<<8)|cpu.regL) - 1) >> 8;//Hl--
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) - 1) & 0xff;//hL--
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--
                    
                    //S, Z, C are not affected.
                    cpu.Flags &= ~0x10; //H is reset.
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags &= ~0x02; //N is reset.
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a+(hl old) bit 1
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a+(hl old) bit 3

                    cpu.cycleCnt += 16;
                    break;
                }
            
                case 0xb8://BLOCK TRANSF INSTRUCTION - while (BC ≠ 0) {(DE) ← (HL), DE ← DE - 1, HL ← HL - 1, BC ← BC – 1}
                {    
                    while(bool(cpu.regB|cpu.regC)) //While BC != 0
                    {
                        uint8_t n = cpu.regA + z80_mem_read((cpu.regH << 8) | cpu.regL);

                        z80_mem_write((cpu.regD<<8)|cpu.regE, z80_mem_read((cpu.regH << 8) | cpu.regL)); //(DE) <- (HL)
                        cpu.regD = (((cpu.regD<<8)|cpu.regE) - 1) >> 8;//De--
                        cpu.regE = (((cpu.regD<<8)|cpu.regE) - 1) & 0xff;//dE--
                        cpu.regH = (((cpu.regH<<8)|cpu.regL) - 1) >> 8;//Hl--
                        cpu.regL = (((cpu.regH<<8)|cpu.regL) - 1) & 0xff;//hL--
                        cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                        cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--
                        
                        //S, Z, C are not affected.
                        cpu.Flags &= ~0x10; //H is reset.
                        (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                        cpu.Flags &= ~0x02; //N is reset.
                        (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a+(hl old) bit 1
                        (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a+(hl old) bit 3

                        cpu.reg_PC -= 2;
                        cpu.cycleCnt += 21;
                    }

                    cpu.cycleCnt += 16; //For BC = 0 check
                    break;
                }
            }

                //SUBTRACTION W CARRY INSTRUCTIONS---------------------------------------------------------
            {

            }

                //ADDITION W CARRY INSTRUCTIONS---------------------------------------------------------
            {

            }

                //WEIRD INSTRUCTIONS------------------------------------------------------------------------------
            {
                case 0x44: //NEG - 2's complement of A
                {    
                    uint8_t temp = cpu.regA; 
                    cpu.regA = subFlags(0, cpu.regA);

                    (temp == 0x80)? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04); //V: set if 80 before comp, reset otherwise
                    (temp != 0x00)? (cpu.Flags |= 0x01):(cpu.Flags &= ~0x01); //C: set if A != 00 pre comp; else reset.

                    cpu.cycleCnt += 8;
                    break;
                }

                case 0x45: //RETURN - PROJECT SPECIFIC :top stack entry popped into PC
                    retS();
                    cpu.cycleCnt += 14;
                    break;

                case 0x4d: //RETURN - PROJECT SPECIFIC :top stack entry popped into PC
                    retS();
                    cpu.cycleCnt += 14;
                    break;
                
                case 0x67: //RRD - (HL)L->AL' (HL)H->(HL)L'  AL->(HL)H' 
                {
                    uint8_t lowHL = z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0xf; //(HL)L
                    uint8_t highHL = z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 4; //(HL)H

                    z80_mem_write(((cpu.regH << 8) | cpu.regL), ((cpu.regA & 0xf)<<4)|highHL); //(HL)' = AL|(HL)H
                    cpu.regA = ((cpu.regA >>4)<<4) | lowHL; //A' = AH|(HL)L

                    cpu.Flags = ZSPXYtable[cpu.regA] | (cpu.Flags & 0x01); //S, Z, Y, X , P normal, cary unaffected
                    cpu.Flags &= ~0x10; //H is reset.
                    cpu.Flags &= ~0x02; //N is reset.

                    cpu.cycleCnt += 18;
                    break;
                }

                case 0x6f: //RLD -   A' = AH|(HL)H     (HL)' = (HL)L|AL
                {
                    uint8_t lowHL = z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0xf; //(HL)L
                    uint8_t highHL = z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 4; //(HL)H

                    z80_mem_write(((cpu.regH << 8) | cpu.regL), (lowHL<<4)|(cpu.regA & 0xf)); //(HL)' = (HL)L|AL
                    cpu.regA = ((cpu.regA >>4)<<4) | highHL; //A' = AH|(HL)H

                    cpu.Flags = ZSPXYtable[cpu.regA] | (cpu.Flags & 0x01); //S, Z, Y, X , P normal, cary unaffected
                    cpu.Flags &= ~0x10; //H is reset.
                    cpu.Flags &= ~0x02; //N is reset.

                    cpu.cycleCnt += 18;
                    break;
                }
            }

                //UNIDENTIFIED INSTRUCTION=================================================================
                default:
                    cout << "Unknown ED Instruction: " << hex << int(inst2) << endl;
                    printReg(cpu);
                    return 1;
                    break;
            }//Close switch

            break; //Break for end of ED instructions in big alpha switch statement
    }




        //UNIDENTIFIED INSTRUCTION----------------------------------------------------------------------------------------------------------------------------------------------------------
        default:
            cout << "Unknown Instruction: " << hex << int(inst) << endl;
            printReg(cpu);
            return 1;
            break;
    }
    }

    return 0;
};





//OTHER FUNCTIONS=====================================================================================================================
//Print out all the registers in the cpu
void printReg(Z80 cpu)
{
    cout << "\n*=================Z80========================*" << endl;
    cout << "Flags:\t" << bitset<8>(cpu.Flags) << endl; // prints in binary to see each bit
    printf("regA:\t0x%02X\n", cpu.regA) ;
    printf("regB:\t0x%02X\n", cpu.regB) ;
    printf("regC:\t0x%02X\n", cpu.regC) ;
    printf("regD:\t0x%02X\n", cpu.regD) ;
    printf("regE:\t0x%02X\n", cpu.regE) ;
    printf("regH:\t0x%02X\n", cpu.regH) ;
    printf("regL:\t0x%02X\n", cpu.regL) ;

    printf("reg_A:\t0x%02X\n", cpu.reg_A);
    printf("reg_F:\t0x%02X\n", cpu.reg_F);
    printf("reg_B:\t0x%02X\n", cpu.reg_B);
    printf("reg_C:\t0x%02X\n", cpu.reg_C);
    printf("reg_D:\t0x%02X\n", cpu.reg_D);
    printf("reg_E:\t0x%02X\n", cpu.reg_E);
    printf("reg_H:\t0x%02X\n", cpu.reg_H);
    printf("reg_L:\t0x%02X\n", cpu.reg_L);

    printf("reg_I:\t0x%02X\n", cpu.reg_I);
    printf("reg_R:\t0x%02X\n", cpu.reg_R);

    printf("reg_IX:\t0x%04X\n", cpu.reg_IX);
    printf("reg_IY:\t0x%04X\n", cpu.reg_IY);
    printf("reg_SP:\t0x%04X\n", cpu.reg_SP);
    printf("reg_PC:\t0x%04X\n", cpu.reg_PC);

    printf("Cycle Count: %d\n", cpu.cycleCnt);
    cout << "*============================================*\n" << endl;
}

//Sebald Version of printing all the registers in the CPU
void printRegTest(Z80 cpu)
{
    cout << "\n*=================Z80========================*" << endl;
    printf("A:\t%02X\n", cpu.regA);
    printf("F:\t%02X\n", cpu.Flags);

    printf("\tS=%01x  Z=%01x  H=%01x  P/V=%01x  N=%01x  C=%01x\n", 
       (cpu.Flags & 0x80) >> 7,  // Sign flag (bit 7)
       (cpu.Flags & 0x40) >> 6,  // Zero flag (bit 6)
       (cpu.Flags & 0x10) >> 4,  // Half-carry flag (bit 4)
       (cpu.Flags & 0x04) >> 2,  // Overflow flag (bit 2)
       (cpu.Flags & 0x02) >> 1,  // Subtract flag (bit 1)
       (cpu.Flags & 0x01));      // Carry flag (bit 0)

    printf("B:\t%02X\n", cpu.regB);
    printf("C:\t%02X\n", cpu.regC);
    printf("D:\t%02X\n", cpu.regD);
    printf("E:\t%02X\n", cpu.regE);
    printf("H:\t%02X\n", cpu.regH);
    printf("L:\t%02X\n", cpu.regL);

    printf("I:\t%02X\n", cpu.reg_I);
    printf("R:\t%02X\n", cpu.reg_R);

    printf("A':\t%02X\n", cpu.reg_A);
    printf("F':\t%02X\n", cpu.reg_F);
    printf("B':\t%02X\n", cpu.reg_B);
    printf("C':\t%02X\n", cpu.reg_C);
    printf("D':\t%02X\n", cpu.reg_D);
    printf("E':\t%02X\n", cpu.reg_E);
    printf("H':\t%02X\n", cpu.reg_H);
    printf("L':\t%02X\n", cpu.reg_L);


    printf("IFF1:\t%01X\n", cpu.regIFF1);
    printf("IFF2:\t%01X\n", cpu.regIFF2);
    printf("IM:\t%01X\n", cpu.regINIR);

    printf("Hidden 16-bit math register:\tNOT IMPLEMENTED\n");

    printf("IX:\t%04X\n", cpu.reg_IX);
    printf("IY:\t%04X\n", cpu.reg_IY);
    printf("PC:\t%04X\n", cpu.reg_PC);
    printf("SP:\t%04X\n", cpu.reg_SP);
    

    printf("Ran %d cycles\n", cpu.cycleCnt);
    cout << "*============================================*\n" << endl;
}


void andFlags(uint8_t reg){

            //flags (move this into a func that gets passed reg a) 
    cpu.Flags |= ~0x01; // sets the carry flag to 0 
    cpu.Flags |= ~0x02; // sets the sub flag 
    (__builtin_popcount(reg) % 2) ? (cpu.Flags |= ~0x04) : (cpu.Flags |= 0x04); // sets the parity flag
    cpu.Flags |= ~0x08;
    (reg = 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags |= ~0x40);
    (reg >= 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags |= ~0x80);

}

void xorFlags(uint8_t reg){

    cpu.Flags |= ~0x01; // sets the carry flag to 0 
    cpu.Flags |= ~0x02; // sets the sub flag 
    (__builtin_popcount(reg) % 2) ? (cpu.Flags |= ~0x04) : (cpu.Flags |= 0x04); // sets the parity flag
    cpu.Flags |= ~0x08;
    (reg = 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags |= ~0x40);
    (reg >= 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags |= ~0x80);

}

//Does the logic for adding a register to another, returns their sum and sets their flags.
uint8_t addFlags(uint8_t reg1, uint8_t reg2)
{
    int16_t sum = reg1 + reg2;

    //cout << bitset<8>(reg1) << " +\n" << bitset<8>(reg2) << " =\n" << bitset<8>(sum) << endl;

    cpu.Flags = ZSXYtable[sum & 0xff]; //Set Z, S, X, Y 
    cpu.Flags |= (sum ^ reg1 ^ reg2) & 0x10; //Half carry: if the sum of the bottom 0xf is greater than 0xf (Demonstrated in class)
    cpu.Flags |= (((~(reg1 ^ reg2) & 0x80) && ((reg1 ^ sum) & 0x80)) << 2); //Overflow: if operants have same sign but sum sign changes (explained by sebald))
    cpu.Flags &= ~ 0x02; //Subtraction: False 
    cpu.Flags |= (sum > 0xff); //Carry: if sum is more than 8 bits can hold then there was a carry

    return sum;
}

//Does the logic for subtracting a register to another, returns their differents and sets their flags. ***NOT FULLY WORKING***
uint8_t subFlags(uint8_t reg1, uint8_t reg2)
{
    uint16_t diff = reg1 - reg2;

    //cout << bitset<8>(reg1) << " -\n" << bitset<8>(reg2) << " =\n" << bitset<8>(diff) << endl;

    cpu.Flags = ZSXYtable[diff & 0xff]; //Set Z, S, X, Y
    cpu.Flags |= (((reg1 & 0x0f) < (reg2 & 0x0f))) << 4; //Half carry: if there was borrow bc thing being subtracted is bigger in lower 4 bits
    cpu.Flags |= (((reg1 ^ reg2) & 0x80) && ((reg1 ^ diff) & 0x80)) << 2; //Overflow: if operants have different sign but diff sign is same as second (explained by sebald)
    cpu.Flags |= 0x02; //Subtraction : True
    cpu.Flags |= (reg2 > reg1) & 0x01; //Carry: If reg 2 is bigger, then there was a borrow

    return diff;
}

//Does the logic for incramenting a register by 1 - Cannot use addFlags, bc carry bit is unaffected
uint8_t incFlags(uint8_t reg1)
{
    int32_t sum = reg1 + uint8_t(1);

    //cout << bitset<8>(reg1) << " +\n" << bitset<8>(1) << " =\n" << bitset<8>(sum) << endl;

    cpu.Flags = ZSXYtable[sum & 0xff] | //Set Z, S, X, Y 
    ((reg1 ^ 0x01 ^ sum) & 0x10)| // Half Carry: Demonstrated in class
    ((reg1 == 0x7F)) << 2 | //Overflow: at 0x80
    //N is reset
    cpu.Flags & 0x01; //CARRY IS UNAFFECTED
    
    return sum;
}

//incrememnts the paired register values - does not impact flags.
uint16_t incPaired(uint8_t regH, uint8_t regL)
{
    uint16_t pairedReg = (regH << 8) | regL; //Combine into paired register
    pairedReg++; //Add one for incrament
    return pairedReg; 
}

//IDoes the logic for adding two 16 bit registers. 
uint16_t add16Flags(uint8_t regH, uint8_t regL, uint16_t other)
{

    uint16_t paired = (regH << 8)| regL; //Make paired 16 bits
    uint32_t sum = paired + other; //Caclilate Sum
    
    //Sign doesnt change
    //Z doesnt change
    cpu.Flags |= ((sum >> 5) & 1) << 5; //5th bit of result
    (((paired & 0x0FFF) + (other & 0x0FFF)) > 0x0FFF) ? (cpu.Flags |=  0x08) : (cpu.Flags &=  ~0x08);//H: Set if carry out of bit 11, reset otherwise
    cpu.Flags |= ((sum >> 3) & 1) << 3; //3rd bit of result
    //P/V doesnt change
    cpu.Flags &= ~0x02; //N reset
    (sum & 0x10000) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //C: set of carry from bit 15, reset otherwise

    return sum & 0xffff;
}

uint8_t twosComp(uint8_t reg)
    {return (~reg) + 1;}

//Decrements registers
uint8_t decFlags(uint8_t reg)
{
    uint8_t diff = reg - uint8_t(1);

    (diff & 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80); //Sign - set bit if diff has sign bit set
    (diff == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40); //Z set if result is 0
    cpu.Flags |= ((diff >> 5) & 1) << 5; //5th bit of result
    ((diff & 0x0f) == 0x0f) ? (cpu.Flags |= 0x08) : (cpu.Flags &= ~0x08); //Half carry if difference has ...1111
    cpu.Flags |= ((diff >> 3) & 1) << 3; //3rd bit of result
    (reg == 0x80) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04); //Overflow if register was already 0
    cpu.Flags |= 0x02; //N set
    //Carry is unaffected

    return diff;
}

//Decrements paired registers - no flags impacted
uint16_t decPaired(uint8_t regH, uint8_t regL)
{
    uint16_t pairedReg = (regH << 8) | regL; //Combine into paired register
    pairedReg--; //Subtract 1 for decrament
    return pairedReg; 
}

//Adds registers, and the carry bit if set
uint8_t adcFlags(uint8_t reg1, uint8_t reg2)
{

    uint16_t sum = reg1 + reg2 + (cpu.Flags & 0x01);

    (sum & 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80); //Sign
    (sum == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40); //Z set if result is 0
    cpu.Flags |= ((sum >> 5) & 1) << 5; //5th bit of result
    ((((reg1 & 0xf) + (reg2 & 0xf) + (cpu.Flags & 0x01)) & 0x10) == 0x10) ? (cpu.Flags |=  0x08) : (cpu.Flags &=  ~0x08); //FROM ROBM.DEV
    cpu.Flags |= ((sum >> 3) & 1) << 3; //3rd bit of result
    ((reg1 >> 7 == reg2 >> 7) && (sum >> 7 != reg1 >> 7)) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04);; //If operants have same sign, but sum sign changes - overflow
    cpu.Flags &= ~0x02; //N reset
    (sum > 0xFF) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //Carry if result is greater than 8 bits can hold
    
    return sum & 0xff;
}

//subtracts registers and the carry bit if set
uint8_t sbcFlags(uint8_t reg1, uint8_t reg2)
{
    uint16_t diff = reg1 - reg2 - (cpu.Flags & 0x01);
    printf("x%02x - x%02x - x%01x = x%02x", reg1, reg2, (cpu.Flags & 0x01), diff);

    (diff & 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80); //Sign
    (diff == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40); //Z set if result is 0
    cpu.Flags |= ((diff >> 5) & 1) << 5; //5th bit of result
    ((reg1 & 0xf) < ((reg2 & 0xf) + (cpu.Flags & 0x01))) ? (cpu.Flags |=  0x08) : (cpu.Flags &=  ~0x08); //FROM ROBM.DEV
    cpu.Flags |= ((diff >> 3) & 1) << 3; //3rd bit of result
    ((reg1 ^ reg2) & (diff ^ reg1) & 0x80) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04);; //If operants have same sign, but sub sign changes - overflow
    cpu.Flags |= 0x02; //N set
    ((reg2 + (cpu.Flags & 0x01))  > reg1) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //Carry if result is greater than 8 bits can hold
    
    return diff & 0xff;
}

//Pop off top of stack into pc
void retS()
{
    uint16_t low = z80_mem_read(cpu.reg_SP++); 
    uint16_t high = z80_mem_read(cpu.reg_SP++); 
    cpu.reg_PC = (high << 8) | low;
}

//pops off top of stack into a register
uint16_t popS()
{
    uint16_t low = z80_mem_read(cpu.reg_SP++); 
    uint16_t high = z80_mem_read(cpu.reg_SP++); 
    return (high << 8) | low;
}

//MAIN==============================================================================================================================
int main(){
    cout << "Max Castle is feeling thankful" << endl; //File running check


   //z80_mem_load(fileRun.c_str()); //Load into memory

   
    /*z80_mem_write(0x00, 0x3e);//load A
    z80_mem_write(0x01, 0x0f);// goes into A*/

    
    /*z80_mem_write(0x00, 0x21);//load HL with n n
    z80_mem_write(0x01, 0xDA);//H
    z80_mem_write(0x02, 0x92);//L*/

    /*z80_mem_write(0x00, 0x31);//load sp with n n
    z80_mem_write(0x01, 0x02);//sp
    z80_mem_write(0x02, 0x01);//sp*/

    /*z80_mem_write(0x00, 0x01);//load BC with n n
    z80_mem_write(0x01, 0x33);//B
    z80_mem_write(0x02, 0x22);//C*/

    /*z80_mem_write(0x03, 0x22);//load nn with HL
    z80_mem_write(0x04, 0x55);//n
    z80_mem_write(0x05, 0x44);//n */

/*
   cpu.regH = 0x11;
   cpu.regL = 0x14;
   cpu.regD = 0x22;
   cpu.regE = 0x25;
   cpu.regB = 0x00;
   cpu.regC = 0x03;
   memory[0x1114] = 0xa5;
   memory[0x1113] = 0x36;
   memory[0x1112] = 0x88;
   memory[0x2225] = 0xc5;
   memory[0x2224] = 0x59;
   memory[0x2223] = 0x66;
   cpu.Flags = 0x00;
*/

    cpu.regH = 0x50;
    cpu.regL = 0x00;
    cpu.regA = 0b01111010;
    memory[0x5000] = 0b00110001;

    //cpu.Flags |= 0x01;
    z80_mem_write(0x00, 0xed);//ed instruction
    z80_mem_write(0x01, 0x6f);//rrd
    

    //z80_mem_write(0x05, 0x84); //a+=h
    //z80_mem_write(0x05, 0x94); //a-=h
    //z80_mem_write(0x05, 0x3c); // a++


    z80_mem_write(0x02, 0x76);//halt
    


    
    /*for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }*/

    z80_execute();

    cout << bitset<8>(memory[0x5000]) << endl;
    
    for (int i =0; i < 3; i++)
    {
        printf("\tram[%04x] = %02x\t ", 0x1114-i, memory[0x1114-i]);
        printf("ram[%04x] = %02x\n ", 0x2225-i, memory[0x2225-i]);
    }


    return 0;
}