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
const string filenameEMMA = "C:\\Users\\ekcha\\OneDrive\\Documents\\GitHub\\411-paper\\load-regs.bin";
const string filenameMAX = "C:\\411\\divide-8.bin"; //MAX, PUT .BIN AFTER THE GODDAMN OATH NAME
const string fileRun = filenameEMMA;


//DEFINING IMPORTNANT THINGS=======================================================================================================
int const CYCLES = 1024;
int const MEMSIZE = 65536;
static uint8_t memory[MEMSIZE];
Z80 cpu;


//FUNCTION DEFINITIONS=========================================
int decode();
void printReg(Z80);
void printRegTest(Z80);
void setflags(uint8_t, bool, bool, bool, bool, bool);
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


//EXECUTION CODES==================================================================================================================
//Run loop
int z80_execute(){

    while(cpu.cycleCnt < CYCLES)
    {
        cpu.reg_R++; //INCRAMENT THE R RESGISTER ON ALL PREFIXES EXCEPT IY CB IX CB
        if(decode()) {break;} // if decode returns 1 (either halt or unknown inst), break
                              // otherwise, keep looping. 
    }
    
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
            cpu.cycleCnt += 4; // 4 Cycles for halt
            //printReg(cpu); //OUR VERSION OF PRINT
            printRegTest(cpu); //SEBALD VERSION OF PRINT
            z80_mem_dump("memory.bin");
            return 1;
            break;

        //NOP INSTUCTION===========================================================================================
        case 0x00: //NOP INSTRUCTION
            cpu.cycleCnt += 4;
            break;
        
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
            subFlags(cpu.reg_A, cpu.reg_HL[0]);
            cpu.cycleCnt+=4;
            break;

        case 0xbf: //cp A from A
            subFlags(cpu.reg_A, cpu.reg_A);
            cpu.cycleCnt+=4;
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





        //SHIFT INSTRUCNTIONS--------------------------------------------------------------------------
        case 0x27: //shift left one byte A
        {
            bool msb = (cpu.regA & 0x80) != 0;
            cpu.reg_A <<= 1;
            (msb) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);
            (cpu.regA == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);  // zero flag
            (cpu.regA & 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80);  // sign flag

            cpu.cycleCnt+=8;

            break;
        }

        case 0x2f: //shift right one byte
         {
            bool lsb = (cpu.regA & 0x01) != 0; // since we shift in this direction it is necessary to 
            //make sure that when shifting, we dont lose any data.
            //lsb flag lets us know if there is a value in the least bit of the word
            
            cpu.regA >>= 1; //actual right shift

            (lsb) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01);  // carry flag
            //if the flag is there, set it. otherwise, set the flag to false

            (cpu.regA == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);  // zero flag
            // the zero flag is the 6th bit is either 1 or 0

            (cpu.regA & 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80);  // sign flag
            //sets the sign flag to 1 or 0 depending on if the sign bit is 1 or 0 

            cpu.cycleCnt += 8;

            break;
         }

         case 0xcb:
            {
                // this just means that we need to prepare to use bit instructions. 
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
            cpu.reg_A = cpu.reg_A & cpu.reg_HL[0]; //bitwise AND for reg a with HL 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xa7: //bitwise and register A with A
            cpu.reg_A = cpu.reg_A & cpu.reg_A; //bitwise AND for reg a with a 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xe6:
            cpu.reg_A = cpu.reg_A & memory[int(cpu.reg_PC++)]; //bitwise AND for reg a with n 
            andFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
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
            cpu.reg_A = cpu.reg_A ^ cpu.reg_HL[0]; //bitwise XOR for reg a with hl 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
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
            cpu.reg_A = cpu.reg_A | cpu.reg_HL[0]; //bitwise OR for reg a with hl 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;

        case 0xb7: // OR A with A 
            cpu.reg_A = cpu.reg_A | cpu.reg_B; //bitwise OR for reg a with a 
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
            break;
        
        case 0xf6:// OR A with n memory[int(cpu.reg_PC++)];
            cpu.reg_A = cpu.reg_A | memory[int(cpu.reg_PC++)]; //bitwise OR for reg a with a             
            xorFlags(cpu.reg_A);
            cpu.cycleCnt += 4;
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


        case 0xcd:
        //unconditional jump call
            cpu.reg_SP = cpu.reg_PC + 3;
            cpu.reg_PC = memory[cpu.reg_PC++];
            cpu.cycleCnt+=17;
            break;

        
        
        // POP FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        /*The memory location pointed to by SP is stored into C and SP is incremented.
         The memory location pointed to by SP is stored into B and SP is incremented again.*/
        case 0xc1:
            cpu.reg_C = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.reg_B = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.cycleCnt += 10;
            break;

        /*The memory location pointed to by SP is stored into E and SP is incremented. 
        The memory location pointed to by SP is stored into D and SP is incremented again.*/
        case 0xd1:
            cpu.reg_C = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.reg_B = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.cycleCnt += 10;
            break;

        /*The memory location pointed to by SP is stored into L and SP is incremented. 
        The memory location pointed to by SP is stored into H and SP is incremented again.*/
        case 0xe1:
            cpu.reg_C = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.reg_B = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.cycleCnt += 10;
            break;

        /*The memory location pointed to by SP is stored into F and SP is incremented. 
        The memory location pointed to by SP is stored into A and SP is incremented again.*/
        case 0xf1:
            cpu.reg_C = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.reg_B = cpu.reg_SP;
            cpu.reg_SP++;

            cpu.cycleCnt += 10;
            break;







        // RETURN FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        /*If the zero flag is unset, the top stack entry is popped into PC.*/
        case 0xc0:
            if ( (cpu.Flags & 0b010000000) == 0)
            {
                cpu.reg_PC = cpu.reg_SP;
                cpu.cycleCnt+=10;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xd0: // carry flag
            if ( (cpu.Flags & 0b000000001) == 0)
            {
                cpu.reg_PC = cpu.reg_SP;
                cpu.cycleCnt+=10;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xe0: // P/V flag
            if ( (cpu.Flags & 0b000000100) == 0)
            {
                cpu.reg_PC = cpu.reg_SP;
                cpu.cycleCnt+=10;
                break;
            }
            cpu.cycleCnt+=5;
            break;

        case 0xf0: // sign flag
            if ( (cpu.Flags & 0b100000000) == 0)
            {
                cpu.reg_PC = cpu.reg_SP;
                cpu.cycleCnt+=10;
                break;
            }
            cpu.cycleCnt+=5;
            break;


        //UNIDENTIFIED INSTRUCTION----------------------------------------------------------------------------------------------------------------------------------------------------------
        default:
            cout << "Unknown Instruction " << hex << int(inst) << endl;
            printReg(cpu);
            return 1;
            break;
    }
    }

    return 0;
}





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



//Edits the flag register according to the arithmetic opperation results
void setflags(uint8_t result, bool negative, bool halfCarry, bool overflow, bool subtraction, bool carry)
{
    //FLAGS: Sign | Zero | UNDOC:Copy of bit 5 of reesult| Half carry | UNDOC:Copy of bit 3 of reesult | Parity/oVerflow | add/subtract N | Carry
    cpu.Flags = 0; //Reset the flags for each result

    if (negative)     {cpu.Flags |= 0b10000000;}  //(bit 7) S-Flag  [0:(+/0)result | 1:(-)result]
    if (result == 0)    {cpu.Flags |= 0b01000000;}  //(bit 6) Z-flag  [0: result!=0 | 1: result==0]
    cpu.Flags |= ((result >> 5) & 1) << 5; //(bit 5) Undoccumented - 5th bit of the result
    if (halfCarry)      {cpu.Flags |= 0b00010000;}  //(bit 4) H-Flag [0:carry absent | 1: carry present]
    cpu.Flags |= ((result >> 3) & 1) << 3; //(bit 3) Undoccumented - 3rd bit of the result
    if(overflow)        {cpu.Flags |= 0b00000100;}  //(bit 2) P/V-Flag [0: no overflow or odd number of 1 bits]
    if(subtraction)     {cpu.Flags |= 0b00000010;}  //(bit 1) N-Flag [0:Addition | 1: Subtraction]
    if(carry)           {cpu.Flags |= 0b00000001;}  //(bit 0) C-Flag [0: no carry/borrow | 1: carry/borrow]
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
    int32_t sum = reg1 + reg2;
    //bool halfCarry = (((reg1 & 0xf) + (reg2 & 0xf)) & 0x10) == 0x10; //FROM ROBM.DEV
    bool halfCarry = (sum ^ reg1 ^ reg2) & 0x10; //Demonstrated in class
    bool overflow = (reg1 >> 7 == reg2 >> 7) && (sum >> 7 != reg1 >> 7); //If operants have same sign, but sum sign changes
    bool carry = sum > 0xFF; //If sum is more than 8 bits can hold, there was a carry
    setflags(sum, (sum < 0), halfCarry, overflow, false, carry);
    return sum;
}

//Does the logic for subtracting a register to another, returns their differents and sets their flags. ***NOT FULLY WORKING***
uint8_t subFlags(uint8_t reg1, uint8_t reg2)
{
    uint16_t diff = reg1 - reg2;
    bool halfCarry = ((reg1 & 0x0f) < (reg2 & 0x0f)); //The thing being subtracted from wasnt big enough: had to borrow
    bool overflow = (((reg1 ^ reg2) & 0x80) && ((reg1 ^ diff) & 0x80)); //If the registers have different signs and the result also has a different sign
    bool carry = reg2 > reg1; //If reg 2 is bigger, then there was a carry
    setflags(diff,(reg2 > reg1), halfCarry, overflow, true, carry);
    return diff;
}

//Does the logic for incramenting a register by 1 - Cannot use addFlags, bc carry bit is unaffected
uint8_t incFlags(uint8_t reg1)
{
    int32_t sum = reg1 + uint8_t(1);
    bool halfCarry = (sum ^ reg1 ^ uint8_t(1)) & 0x10; //Demonstrated in class
    bool overflow = (reg1 == 0x7F); //Overflows at 0x80
    bool carry = cpu.Flags & 0x01; //CARRY IS UNAFFECTED
    setflags(sum, (sum < 0), halfCarry, overflow, false, carry);
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

//MAIN==============================================================================================================================
int main(){
    cout << "Max Castle is feeling thankful" << endl; //File running check

    //z80_mem_load(fileRun.c_str()); //Load into memory
    z80_mem_write(0x00, 0x26);//load H with n
    z80_mem_write(0x01, 0x55);//H
    z80_mem_write(0x02, 0x2e);//load L with n
    z80_mem_write(0x03, 0x44);//L
    
    //cpu.Flags |= 0x01; ///set carry
    z80_mem_write(0x04, 0x3e);//load A
    z80_mem_write(0x05, 0x10);// goes into A

    /*z80_mem_write(0x06, 0x06);//load B with n
    z80_mem_write(0x07, 0x04);//B

    z80_mem_write(0x08, 0x36);//load (HL) with n
    z80_mem_write(0x09, 0xff);//n

    z80_mem_write(0x0a, 0x86);//a += (HL)

    z80_mem_write(0x0b, 0x88);//regA += b + carry*/

    z80_mem_write(0x06, 0xde); //regA -= n + carry
    z80_mem_write(0x07, 0x20); // n

    z80_mem_write(0x08, 0x76);//halt

     
    
    for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }

    z80_execute();


    for (int i =0x5540; i < 0x5547; i++)
    {
        {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }
    printf("num: %02x", z80_mem_read((cpu.regH << 8)|cpu.regL));

    return 0;
}