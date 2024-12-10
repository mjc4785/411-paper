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
const string filenameEMMA = "C:\\Users\\ekcha\\OneDrive\\Documents\\GitHub\\411-paper\\divide-8.bin";
const string filenameMAX = "C:\\411\\offset-bit.bin"; //MAX, PUT .BIN AFTER THE GODDAMN OATH NAME
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
uint8_t andFlags(uint8_t, uint8_t);
void xorFlags(uint8_t);
uint16_t incPaired(uint8_t, uint8_t);
uint16_t add16Flags(uint8_t, uint8_t, uint16_t);
uint8_t decFlags(uint8_t);
uint16_t decPaired(uint8_t, uint8_t);
uint8_t adcFlags(uint8_t, uint8_t);
uint8_t sbcFlags(uint8_t, uint8_t);
void retS();
uint16_t popS();
void push(uint16_t);
uint16_t sbc16Flags(uint16_t, uint16_t);
uint16_t adc16Flags(uint16_t, uint16_t);
void incR();
uint16_t displ2(uint16_t , uint8_t );

uint8_t sraFlags(uint8_t);
uint8_t slaFlags(uint8_t);
uint8_t srlFlags(uint8_t);
uint8_t sllFlags(uint8_t);

uint8_t rlcFlags(uint8_t);
uint8_t rrcFlags(uint8_t);
uint8_t rlFlags(uint8_t);
uint8_t rrFlags(uint8_t);

void bitZero(uint8_t);
void bitOne(uint8_t);
void bitTwo(uint8_t);
void bitThree(uint8_t);
void bitFour(uint8_t);
void bitFive(uint8_t);
void bitSix(uint8_t);
void bitSeven(uint8_t);

//OUT OF SIGHT OUT OF MIND (DONT TOUCH THESE I DIDNT WRITE THEM)====================================================================
void z80_mem_write(uint16_t addr, uint8_t value) {
    memory[addr] = value;
}

uint8_t z80_mem_read(uint16_t addr) {
    return memory[addr];
}

//Writes xAB as xB xA in memory
void z80_mem_write16(uint16_t addr, uint16_t value) {
    memory[addr] = (uint8_t)(value & 0xff);
    memory[addr + 1] = (uint8_t)(value >> 8);
}

//Reads in xA xB as xBA, does not incrament PC, should add 2 after calling for instruction read in
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
        incR(); //INCRAMENT THE R RESGISTER ON ALL PREFIXES EXCEPT IY CB IX CB, 7th bit doesnt change
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
            
            if(!cpu.regIFF1 && !cpu.regIFF2)
            {
                cpu.cycleCnt += 4; // 4 Cycles for halt
                //printReg(cpu); //OUR VERSION OF PRINT
                printRegTest(cpu); //SEBALD VERSION OF PRINT
                z80_mem_dump("memory.bin");
                return 1;
            }

            return 0;
            
            break;



        // DAA inst ================================================================================================================================================================
        
        case 0x27:
        {

            //initialize things needed for the operation
            int low = cpu.regA & 0x0F;                          //initialize the lower four bits of A
            int high = cpu.regA >> 4;                           //shiftit over to the right four spaces to make room for higher bits
            int cf = cpu.regA & 0x01;                           // set the carry flag  
            int hf = cpu.regA & 0x10;                           // set the half carry flag
            int nf = cpu.regA & 0x02;                           // set the add/sub flag
            uint16_t _value = 0;                                // initialize the value int so that at the end we can use it

            if(cf) {                                            //if the carry flag is set
                _value = (low < 0x0A && !hf) ? 0x60 : 0x66;     
            }
            else {                                              //if carry flag isnt set
                if(low < 0x0A) {
                    if(high < 0x0A) {
                        _value = (hf) ? 0x06 : 0x00;
                    }
                    else {
                        _value = (hf) ? 0x66 : 0x60;
                    }
                }
                else {
                    _value = (high < 0x09) ? 0x06 : 0x66;
                }
            }

            if(nf) {                                            //if the add/sub flag is set 
                cpu.regA -= _value;
            }
            else {
                cpu.regA += _value;
            }

            cpu.regA = ZSPXYtable[cpu.regA] | (nf);

            if(_value >= 0x60)                                  // if the value is greater than or = to carry without half carry  
                cpu.regA |= 0x01;

            if(nf) {                                            //if the add/sub flag is set 
                if(hf && low < 0x06) {
                    cpu.regA |= 0x10;
                }
            }
            else if(low >= 10) {
                cpu.regA |= 0x10;
            }

            cpu.cycleCnt += 4;
            break;
        }

        //================================================================================================================================================================



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
        
        case 0x31: //LOAD INSTRUCTION - Load 16 bit value nn into sp
            cpu.reg_SP = z80_mem_read16(cpu.reg_PC);
            cpu.reg_PC += 2; //next instruction
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
        
        case 0x86: //ADD INSTRUCTION - RegA += memory[regHL]
            cpu.regA = addFlags(cpu.regA,z80_mem_read((((cpu.regH << 8) | cpu.regL))));
            cpu.cycleCnt += 7;
            break;
        
        case 0xc6: //ADD IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = addFlags(cpu.regA,z80_mem_read(int(cpu.reg_PC++)));
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
            subFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt+=4;
            break;

        case 0xb9: //cp C from A
            subFlags(cpu.regA, cpu.regC);
            cpu.cycleCnt+=4;
            break;

        case 0xba: //cp D from A
            subFlags(cpu.regA, cpu.regD);
            cpu.cycleCnt+=4;
            break;

        case 0xbb: //cp E from A
            subFlags(cpu.regA, cpu.regE);
            cpu.cycleCnt+=4;
            break; 

        case 0xbc: //cp H from A
            subFlags(cpu.regA, cpu.regH);
            cpu.cycleCnt+=4;
            break;

        case 0xbd: //cp L from A
            subFlags(cpu.regA, cpu.regL);
            cpu.cycleCnt+=4;
            break;

        case 0xbe: //cp HL from A
            subFlags(cpu.regA, z80_mem_read(((cpu.regH << 8) | cpu.regL)));
            cpu.cycleCnt+=7;
            break;

        case 0xbf: //cp A from A
            subFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt+=4;
            break;
        
        case 0xfe: //cp n from A
            subFlags(cpu.regA, z80_mem_read(cpu.reg_PC++));
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


// rst 00 ops 


        case 0xc7: // 00h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 0;
            break;

        case 0xd7: // 10h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 16;
            break;
 
        case 0xe7: // 20h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 32;
            break;

        case 0xf7: // 30h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 48;
            break;






        case 0xcf: // 08h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 8;
            break;

        case 0xdf: // 18h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 24;
            break;

        case 0xef: // 28h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 40;
            break;

        case 0xff: // 38h
            cpu.reg_SP = ++cpu.reg_PC;
            cpu.reg_PC = 56;
            break;











        // JUMP INST -----------------------------------------------------------------------------------------------------------------

        case 0x10: // jump if zero is not set after decrementing b reg 
        {
            cpu.regB--; // Decrement the B register
            // Read the offset as unsigned but interpret it as signed
            int8_t offset = static_cast<int8_t>(z80_mem_read(int(cpu.reg_PC))); 
            cpu.reg_PC++; // Increment PC to move past the offset byte

            if (cpu.regB != 0) {
                cpu.reg_PC += offset; // Apply the signed relative jump
                cpu.cycleCnt += 13; // Jump takes 13 cycles
            } else {
                cout << "No jump, B = 0" << endl;
                cpu.cycleCnt += 8; // No jump takes 8 cycles
            }
            break;
        }

        case 0x20: //if Z flag unset, shift PC by d
        {
            int8_t offset = static_cast<int8_t>(z80_mem_read(int(cpu.reg_PC))); 
            cpu.reg_PC++; // Increment PC to move past the offset byte
            
            if ((cpu.Flags & 0x40) == 0)
            {
                cpu.reg_PC += offset;
                cpu.cycleCnt += 12; 
                break;
            
            }
            cpu.cycleCnt+=7;
            break;
        }

            
        case 0x30: //if C flag unset, shift PC by d
        {
            int8_t offset = static_cast<int8_t>(z80_mem_read(int(cpu.reg_PC))); 
            cpu.reg_PC++; // Increment PC to move past the offset byte
            
            if ((cpu.Flags & 0x01) == 0)
            {
                cpu.reg_PC += offset;
                cpu.cycleCnt += 12; 
                break;
            
            }
            cpu.cycleCnt+=7;
            break;
        }



        case 0x18: // shift PC by d
            cpu.reg_PC += z80_mem_read(int(cpu.reg_PC++));
            cpu.cycleCnt += 12; 
            break;



        case 0x28: //if Z flag set, shift PC by d
            {
            int8_t offset = static_cast<int8_t>(z80_mem_read(int(cpu.reg_PC))); 
            cpu.reg_PC++; // Increment PC to move past the offset byte
            
            if ((cpu.Flags & 0x40) != 0)
            {
                cpu.reg_PC += offset;
                cpu.cycleCnt += 12; 
                break;
            
            }
            cpu.cycleCnt+=7;
            break;
        }



        case 0x38: //if C flag set, shift PC by d
        {
            int8_t offset = static_cast<int8_t>(z80_mem_read(int(cpu.reg_PC))); 
            cpu.reg_PC++; // Increment PC to move past the offset byte
            
            if ((cpu.Flags & 0x01) != 0)
            {
                cpu.reg_PC += offset;
                cpu.cycleCnt += 12; 
                break;
            
            }
            cpu.cycleCnt+=7;
            break;
        }


        //JP cc, nn---------------------------------------------------------------------
        case 0xc2: // if the Z flag is unset, jump to nn
            if (!bool(cpu.Flags & 0x40))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xd2: // if the C flag is unset, jump to nn
            if (!bool(cpu.Flags & 0x01))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xe2: // if the P/V flag is unset, jump to nn
            if (!bool(cpu.Flags & 0x04))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xf2: // if the sign flag is unset, jump to nn
            if (!bool(cpu.Flags & 0x80))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xca: // if the Z flag is set, jump to nn
            if (bool(cpu.Flags & 0x40))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xda: // if the C flag is set, jump to nn
            if (bool(cpu.Flags & 0x01))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xea: // if the P/V flag is set, jump to nn
            if (bool(cpu.Flags & 0x04))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        case 0xfa: // if the sign flag is set, jump to nn
            if (bool(cpu.Flags & 0x80))
                {cpu.reg_PC = z80_mem_read16(cpu.reg_PC);} //jump
            else
                {cpu.reg_PC += 2;} //move on
            cpu.cycleCnt+=10;
            break;

        //JP nn------------------------------------------------------------------------
        case 0xc3: //jumps to nn 
            //The first operand in this assembled object code is the low-order byte of a two-byte address.
            cpu.reg_PC = z80_mem_read16(cpu.reg_PC);
            cpu.cycleCnt += 10; 
            break;
    


       













        case 0xcb: // if called, bit instructions 
        {

            incR(); //whatever is in paper - 1 (cause one already added prior)

            uint8_t inst2 = z80_mem_read(cpu.reg_PC++); //Reads the next instuction and incraments program counter

            switch(inst2)
            {

        //SHIFT INSTRUCNTIONS-------------------------------------------------------------------------

                    case 0x00: // RLC b 
                        cpu.regB = rlcFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x01: // RLC c
                        cpu.regC = rlcFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x02: // RLC d
                        cpu.regD = rlcFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x03: // RLC e
                        cpu.regE = rlcFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x04: // RLC h
                        cpu.regH = rlcFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x05: // RLC l
                        cpu.regL = rlcFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x06: // RLC (hl) z80_mem_write( ((cpu.regH << 8) | cpu.regL), sraFlags((z80_mem_read(((cpu.regH << 8) | cpu.regL))))) ;

                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), rlcFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL))));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x07: // RLC a
                        cpu.regA = rlcFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;







                    case 0x08: //RRC b
                        cpu.regB = rrcFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x09: //RRC c
                        cpu.regC = rrcFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0A: //RRC d
                        cpu.regD = rrcFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0B: //RRC e
                        cpu.regE = rrcFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0C: //RRC h
                        cpu.regH = rrcFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0D: //RRC l
                        cpu.regL = rrcFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x0E: //RRC (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), (rrcFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x0F: //RRC a
                        cpu.regA = rrcFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;






                    case 0x10: // rl b

                        {
                        cpu.regB = rlFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x11: // rl c

                        {
                        cpu.regC = rlFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x12: // rl d
                    
                        {
                        cpu.regD = rlFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x13: // rl e
                                        
                        {
                        cpu.regE = rlFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        }
                        
                        break; 

                    case 0x14: // rl h
                                        
                        {
                        cpu.regH = rlFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        }
                        
                        break;

                    case 0x15: // rl l
                                        
                        {
                        cpu.regL = rlFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        }
                        
                        break;

                    case 0x16: // rl (hl)
                                        
                        {
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), (rlFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1)));
                        cpu.cycleCnt+=15;
                        }
                        
                        break;

                    case 0x17: // rl a
                                        
                        {
                        cpu.regA = rlFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        }
                        
                        break;









                    case 0x18: // rr b 

                        {
                        cpu.regA = rrFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x19: // rr c
                    
                        {
                            cpu.regC = rrFlags(cpu.regC);
                            cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1A: // rr d
                                        
                        {
                        cpu.regD = rrFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1B: // rr e
                                        
                        {
                        cpu.regE = rrFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1C: // rr h
                                        
                        {
                        cpu.regH = rrFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1D: // rr l
                                        
                        {
                        cpu.regL = rrFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        }

                        break;

                    case 0x1E: // rr (hl)
                                        
                        {
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), (rrFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL)) >> 1)));
                        cpu.cycleCnt+=15;
                        }
                        
                        break;

                    case 0x1F: // rr a
                                        
                        {
                        cpu.regA = rrFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        }

                        break;







                    case 0x20: //sla b
                        cpu.regB = slaFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x21: //sla c
                        cpu.regC = slaFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x22: //sla d
                        cpu.regD = slaFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x23: //sla e
                        cpu.regE = slaFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x24: //sla h
                        cpu.regH = slaFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x25: //sla l
                        cpu.regL = slaFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x26: //sla (hl)
                       (((z80_mem_read(((cpu.regH << 8) | cpu.regL)) & 0x80) == 0x80) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01));
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), ((z80_mem_read(((cpu.regH << 8) | cpu.regL)) << 1)));
                        cpu.Flags = ZSPXYtable[int((z80_mem_read(((cpu.regH << 8) | cpu.regL))))]; //Set Z, S, X, Y
                        cpu.cycleCnt+=15;
                        break; 

                    case 0x27: //sla a
                        cpu.regA = slaFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;






                    case 0x28: // sra b
                        cpu.regB = sraFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x29: // sra c
                        cpu.regC = sraFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2A: // sra d
                        cpu.regD = sraFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2B: // sra e
                        cpu.regE = sraFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2C: // sra h
                        cpu.regH = sraFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2D: // sra l
                        cpu.regL = sraFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x2E: // sra (hl)
                        z80_mem_write( ((cpu.regH << 8) | cpu.regL), sraFlags((z80_mem_read(((cpu.regH << 8) | cpu.regL))))) ;
                        cpu.cycleCnt+=15;
                        break;
 
                    case 0x2F: // sra a
                        cpu.regA = sraFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;







                    case 0x30: // sll b 
                        cpu.regB = sllFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x31: // sll c
                        cpu.regC = sllFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x32: // sll d 
                        cpu.regD = sllFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x33: // sll e 
                        cpu.regE = sllFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x34: // sll h 
                        cpu.regH = sllFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x35: // sll l 
                        cpu.regL = sllFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;
                         
                    case 0x36: // sll (hl) 
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), sllFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL))));
                        cpu.cycleCnt+=15;
                        break;
                        
                    case 0x37: // sll a
                        cpu.regA = sllFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                        
                    case 0x38: // srl b
                        cpu.regB = srlFlags(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x39: // srl c
                        cpu.regC = srlFlags(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3A: // srl d
                        cpu.regD = srlFlags(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3B: // srl e
                        cpu.regE = srlFlags(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3C: // srl h
                        cpu.regH = srlFlags(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3D: // srl l
                        cpu.regL = srlFlags(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;
                        
                    case 0x3E: // srl (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)), srlFlags(z80_mem_read(((cpu.regH << 8) | cpu.regL))));
                        cpu.cycleCnt+=15;
                        break;
                        
                    case 0x3F: // srl a
                        cpu.regA = srlFlags(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;
                        



// bit test 



                    case 0x40: // bit 0, b
                        bitZero(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x41: // bit 0, c
                        bitZero(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x42: // bit 0, d
                        bitZero(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x43: // bit 0, e
                        bitZero(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x44: // bit 0, h
                        bitZero(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x45: // bit 0, l
                        bitZero(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x46: // bit 0, (hl)
                        bitZero(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x47: // bit 0, a
                        bitZero(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x48: // bit 1, b
                        bitOne(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x49: // bit 1, c
                        bitOne(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x4A: // bit 1, d
                        bitOne(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x4B: // bit 1, e
                        bitOne(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x4C: // bit 1, h
                        bitOne(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x4D: // bit 1, l
                        bitOne(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x4E: // bit 1, (hl)
                        bitOne(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x4F: // bit 1, a
                        bitOne(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x50: // bit 2, b
                        bitTwo(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x51: // bit 2, c
                        bitTwo(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x52: // bit 2, d
                        bitTwo(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x53: // bit 2, e
                        bitTwo(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x54: // bit 2, h
                        bitTwo(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x55: // bit 2, l
                        bitTwo(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x56: // bit 2, (hl)
                        bitTwo(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x57: // bit 2, a
                        bitTwo(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x58: // bit 3, b
                        bitThree(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x59: // bit 3, c
                        bitThree(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x5A: // bit 3, d
                        bitThree(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x5B: // bit 3, e
                        bitThree(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x5C: // bit 3, h
                        bitThree(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x5D: // bit 3, l
                        bitThree(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x5E: // bit 3, (hl)
                        bitThree(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x5F: // bit 3, a
                        bitThree(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x60: // bit 4, b
                        bitFour(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x61: // bit 4, c
                        bitFour(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x62: // bit 4, d
                        bitFour(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x63: // bit 4, e
                        bitFour(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x64: // bit 4, h
                        bitFour(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x65: // bit 4, l
                        bitFour(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x66: // bit 4, (hl)
                        bitFour(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;
 
                    case 0x67: // bit 4, a
                        bitFour(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x68: // bit 5, b
                        bitFive(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x69: // bit 5, c
                        bitFive(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x6A: // bit 5, d
                        bitFive(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x6B: // bit 5, e
                        bitFive(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x6C: // bit 5, h
                        bitFive(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x6D: // bit 5, l
                        bitFive(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x6E: // bit 5, (hl)
                        bitFive(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x6F: // bit 5, a
                        bitFive(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x70: // bit 6, b
                        bitSix(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x71: // bit 6, c
                        bitSix(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x72: // bit 6, d
                        bitSix(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x73: // bit 6, e
                        bitSix(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x74: // bit 6, h
                        bitSix(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x75: // bit 6, l
                        bitSix(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x76: // bit 6, (hl)
                        bitSix(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x77: // bit 6, a
                        bitSix(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;



                    case 0x78: // bit 7, b
                        bitSeven(cpu.regB);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x79: // bit 7, c
                        bitSeven(cpu.regC);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x7A: // bit 7, d
                        bitSeven(cpu.regD);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x7B: // bit 7, e
                        bitSeven(cpu.regE);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x7C: // bit 7, h
                        bitSeven(cpu.regH);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x7D: // bit 7, l
                        bitSeven(cpu.regL);
                        cpu.cycleCnt+=8;
                        break;

                    case 0x7E: // bit 7, (hl)
                        bitSeven(z80_mem_read(((cpu.regH << 8) | cpu.regL)));
                        cpu.cycleCnt+=15;
                        break;

                    case 0x7F: // bit 7, a
                        bitSeven(cpu.regA);
                        cpu.cycleCnt+=8;
                        break;


// reset 


                    case 0x80: // res 0, b
                        cpu.regB &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x81: // res 0, c
                        cpu.regC &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x82: // res 0, d
                        cpu.regD &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x83: // res 0, e
                        cpu.regE &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x84: // res 0, h
                        cpu.regH &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x85: // res 0, l
                        cpu.regL &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x86: // res 0, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x01);
                        cpu.cycleCnt+=15;
                        break;
 
                    case 0x87: // res 0, a
                        cpu.regA &= ~0x01;
                        cpu.cycleCnt+=8;
                        break;



                    case 0x88: // res 1, b
                        cpu.regB &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x89: // res 1, c
                        cpu.regC &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x8A: // res 1, d
                        cpu.regD &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x8B: // res 1, e
                        cpu.regE &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x8C: // res 1, h
                        cpu.regH &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x8D: // res 1, l
                        cpu.regL &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x8E: // res 1, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x02);
                        cpu.cycleCnt+=15;
                        break;

                    case 0x8F: // res 1, a
                        cpu.regA &= ~0x02;
                        cpu.cycleCnt+=8;
                        break;



                    case 0x90: // res 2, b
                        cpu.regB &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x91: // res 2, c
                        cpu.regC &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x92: // res 2, d
                        cpu.regD &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x93: // res 2, e
                        cpu.regE &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x94: // res 2, h
                        cpu.regH &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x95: // res 2, l
                        cpu.regL &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x96: // res 2, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x04);
                        cpu.cycleCnt+=15;
                        break;

                    case 0x97: // res 2, a
                        cpu.regA &= ~0x04;
                        cpu.cycleCnt+=8;
                        break;



                    case 0x98: // res 3, b
                        cpu.regB &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x99: // res 3, c
                        cpu.regC &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x9A: // res 3, d
                        cpu.regD &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x9B: // res 3, e
                        cpu.regE &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x9C: // res 3, h
                        cpu.regH &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x9D: // res 3, l
                        cpu.regL &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0x9E: // res 3, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x08);
                        cpu.cycleCnt+=15;
                        break;

                    case 0x9F: // res 3, a
                        cpu.regA &= ~0x08;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xA0: // res 4, b
                        cpu.regB &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA1: // res 4, c
                        cpu.regC &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA2: // res 4, d
                        cpu.regD &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA3: // res 4, e
                        cpu.regE &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA4: // res 4, h
                        cpu.regH &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA5: // res 4, l
                        cpu.regL &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA6: // res 4, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x10);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xA7: // res 4, a
                        cpu.regA &= ~0x10;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xA8: // res 5, b
                        cpu.regB &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xA9: // res 5, c
                        cpu.regC &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xAA: // res 5, d
                        cpu.regD &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xAB: // res 5, e
                        cpu.regE &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xAC: // res 5, h
                        cpu.regH &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xAD: // res 5, l
                        cpu.regL &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xAE: // res 5, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x20);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xAF: // res 5, a
                        cpu.regA &= ~0x20;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xB0: // res 6, b
                        cpu.regB &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB1: // res 6, c
                        cpu.regC &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB2: // res 6, d
                        cpu.regD &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB3: // res 6, e
                        cpu.regE &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB4: // res 6, h
                        cpu.regH &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB5: // res 6, l
                        cpu.regL &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB6: // res 6, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x40);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xB7: // res 6, a
                        cpu.regA &= ~0x40;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xB8: // res 7, b
                        cpu.regB &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xB9: // res 7, c
                        cpu.regC &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xBA: // res 7, d
                        cpu.regD &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xBB: // res 7, e
                        cpu.regE &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xBC: // res 7, h
                        cpu.regH &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xBD: // res 7, l
                        cpu.regL &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xBE: // res 7, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) & ~0x80);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xBF: // res 7, a
                        cpu.regA &= ~0x80;
                        cpu.cycleCnt+=8;
                        break;


// set 


                    case 0xC0:// set 0, b
                        cpu.regB |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC1:// set 0, c
                        cpu.regC |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC2:// set 0, d
                        cpu.regD |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC3:// set 0, e
                        cpu.regE |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC4:// set 0, h
                        cpu.regH |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC5:// set 0, l
                        cpu.regL |= 0x01;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC6: // set 0, hl
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x01);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xC7:// set 0, a
                        cpu.regA |= 0x01;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xC8: // set 1, b
                        cpu.regB |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xC9: // set 1, c
                        cpu.regC |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xCA: // set 1, d
                        cpu.regD |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xCB: // set 1, e
                        cpu.regE |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xCC: // set 1, h
                        cpu.regH |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xCD: // set 1, l
                        cpu.regL |= 0x02;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xCE: // set 1, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x02);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xCF: // set 1, a
                        cpu.regA |= 0x02;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xD0: // set 2, b
                        cpu.regB |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD1: // set 2, c
                        cpu.regC |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD2: // set 2, d
                        cpu.regD |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD3: // set 2, e
                        cpu.regE |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD4: // set 2, h
                        cpu.regH |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD5: // set 2, l
                        cpu.regL |= 0x04;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD6: // set 2, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x04);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xD7: // set 2, a
                        cpu.regA |= 0x04;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xD8: // set 3, b
                        cpu.regB |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xD9: // set 3, c
                        cpu.regC |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xDA: // set 3, d
                        cpu.regD |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xDB: // set 3, e
                        cpu.regE |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xDC: // set 3, h
                        cpu.regH |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xDD: // set 3, l
                        cpu.regL |= 0x08;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xDE: // set 3, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x08);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xDF: // set 3, a
                        cpu.regA |= 0x08;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xE0: // set 4, b
                        cpu.regB |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE1: // set 4, c
                        cpu.regC |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE2: // set 4, d
                        cpu.regD |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE3: // set 4, e
                        cpu.regE |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE4: // set 4, h
                        cpu.regH |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE5: // set 4, l
                        cpu.regL |= 0x10;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE6: // set 4, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x10);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xE7: // set 4, a
                        cpu.regA |= 0x10;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xE8: // set 5, b
                        cpu.regB |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xE9: // set 5, c
                        cpu.regC |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xEA: // set 5, d
                        cpu.regD |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xEB: // set 5, e
                        cpu.regE |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xEC: // set 5, h
                        cpu.regH |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xED: // set 5, l
                        cpu.regL |= 0x20;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xEE: // set 5, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x20);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xEF: // set 5, a
                        cpu.regA |= 0x20;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xF0: // set 6, b
                        cpu.regB |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF1: // set 6, c
                        cpu.regC |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF2: // set 6, d
                        cpu.regD |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF3: // set 6, e
                        cpu.regE |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF4: // set 6, h
                        cpu.regH |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF5: // set 6, l
                        cpu.regL |= 0x40;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF6: // set 6, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x40);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xF7: // set 6, a
                        cpu.regA |= 0x40;
                        cpu.cycleCnt+=8;
                        break;



                    case 0xF8: // set 7, b
                        cpu.regB |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xF9: // set 7, c
                        cpu.regC |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xFA: // set 7, d
                        cpu.regD |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xFB: // set 7, e
                        cpu.regE |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xFC: // set 7, h
                        cpu.regH |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xFD: // set 7, l
                        cpu.regL |= 0x80;
                        cpu.cycleCnt+=8;
                        break;

                    case 0xFE: // set 7, (hl)
                        z80_mem_write(z80_mem_read(((cpu.regH << 8) | cpu.regL)) , z80_mem_read(((cpu.regH << 8) | cpu.regL)) | 0x80);
                        cpu.cycleCnt+=15;
                        break;

                    case 0xFF: // set 7, a
                        cpu.regA |= 0x80;
                        cpu.cycleCnt+=8;
                        break;



                }
                break;
            }




// finished with CB table



        



        //AND INSTRUCTIONS -----------------------------------------------
        case 0xa0: //bitwise and register A with B
            cpu.regA = andFlags(cpu.regA, cpu.regB); //bitwise AND for reg a with b 
            cpu.cycleCnt += 4;
            break;

        case 0xa1: //bitwise and register A with C
            cpu.regA = andFlags(cpu.regA, cpu.regC); //bitwise AND for reg a with c
            cpu.cycleCnt += 4;
            break;

        case 0xa2: //bitwise and register A with D
            cpu.regA = andFlags(cpu.regA, cpu.regD); //bitwise AND for reg a with D
            cpu.cycleCnt += 4;
            break;

        case 0xa3: //bitwise and register A with E
            cpu.regA = andFlags(cpu.regA, cpu.regE); //bitwise AND for reg a with E
            cpu.cycleCnt += 4;
            break;

        case 0xa4: //bitwise and register A with H
            cpu.regA = andFlags(cpu.regA, cpu.regH); //bitwise AND for reg a with H
            cpu.cycleCnt += 4;
            break;

        case 0xa5: //bitwise and register A with L
            cpu.regA = andFlags(cpu.regA, cpu.regL); //bitwise AND for reg a with L
            cpu.cycleCnt += 4;
            break;

        case 0xa6: //bitwise and register A with HL
            cpu.regA = andFlags(cpu.regA, z80_mem_read(((cpu.regH << 8) | cpu.regL))); //bitwise AND for reg a with HL 
            cpu.cycleCnt += 7;
            break;

        case 0xa7: //bitwise and register A with A
            cpu.regA = andFlags(cpu.regA, cpu.regA); //bitwise AND for reg a with A
            cpu.cycleCnt += 4;
            break;

        case 0xe6:
            cpu.regA = andFlags(cpu.regA, memory[int(cpu.reg_PC++)]); //bitwise AND for reg a with n 
            cpu.cycleCnt += 7;
            break;





        //XOR Instruction --------------------------------------------------------
        case 0xa8: // XOR A with B 
            cpu.regA = cpu.regA ^ cpu.regB; //bitwise XOR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xa9: // XOR A with C 
            cpu.regA = cpu.regA ^ cpu.regC; //bitwise XOR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xaa: // XOR A with D 
            cpu.regA = cpu.regA ^ cpu.regD; //bitwise XOR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xab: // XOR A with E 
            cpu.regA = cpu.regA ^ cpu.regE; //bitwise XOR for reg a with e  
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xac: // XOR A with H 
            cpu.regA = cpu.regA ^ cpu.regH; //bitwise XOR for reg a with h 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xad: // XOR A with L 
            cpu.regA = cpu.regA ^ cpu.regL; //bitwise XOR for reg a with l 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xae: // XOR A with HL 
            cpu.regA = cpu.regA ^ z80_mem_read(((cpu.regH << 8) | cpu.regL)); //bitwise XOR for reg a with hl 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 7;
            break;

        case 0xaf: // XOR A with A 
            cpu.regA = cpu.regA ^ cpu.regA; //bitwise XOR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xee:
            cpu.regA = cpu.regA ^ z80_mem_read(int(cpu.reg_PC++));
            xorFlags(cpu.regA);
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
            cpu.regA = cpu.regA | cpu.regB; //bitwise OR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb1: // OR A with C 
            cpu.regA = cpu.regA | cpu.regC; //bitwise OR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb2: // OR A with D 
            cpu.regA = cpu.regA | cpu.regD; //bitwise OR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb3: // OR A with E 
            cpu.regA = cpu.regA | cpu.regE; //bitwise OR for reg a with e  
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb4: // OR A with H 
            cpu.regA = cpu.regA | cpu.regH; //bitwise OR for reg a with h 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb5: // OR A with L 
            cpu.regA = cpu.regA | cpu.regL; //bitwise OR for reg a with l 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;

        case 0xb6: // OR A with HL 
            cpu.regA = cpu.regA | z80_mem_read(((cpu.regH << 8) | cpu.regL)); //bitwise OR for reg a with hl 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 7;
            break;

        case 0xb7: // OR A with A 
            cpu.regA = cpu.regA | cpu.regA; //bitwise OR for reg a with a 
            xorFlags(cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0xf6:// OR A with n memory[int(cpu.reg_PC++)];
            cpu.regA = cpu.regA | z80_mem_read(int(cpu.reg_PC++)); //bitwise OR for reg a with a             
            xorFlags(cpu.regA);
            cpu.cycleCnt += 7;
            break;

        
        // PUSH FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 0xc5: //PUSH BC
            z80_mem_write( --cpu.reg_SP, cpu.regB );
            z80_mem_write( --cpu.reg_SP, cpu.regC );
            cpu.cycleCnt += 11;
            break;

        case 0xd5: //PUSH DE
            z80_mem_write( --cpu.reg_SP, cpu.regD );
            z80_mem_write( --cpu.reg_SP, cpu.regE );
            cpu.cycleCnt += 11;
            break;

        case 0xe5: //PUSH HL
            z80_mem_write( --cpu.reg_SP, cpu.regH );
            z80_mem_write( --cpu.reg_SP, cpu.regL );
            cpu.cycleCnt += 11;
            break;
        
        case 0xf5: //PUSH AF
            z80_mem_write( --cpu.reg_SP, cpu.regA );
            z80_mem_write( --cpu.reg_SP, cpu.Flags );
            cpu.cycleCnt += 11;
            break;

        
        // CALL FUNCS ------------------------------------------------------------------------------------------------------------------------------------------------------------
        case 0xc4: //If the zero flag is unset
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x40) == 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xd4: //If the carry flag is unset
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x10) == 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xe4: //If the P/V flag is unset
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x04) == 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xf4: //If the sign flag is unset
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x80) == 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xcc: //If the zero flag is set
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x40) != 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xdc: //If the carry flag is set
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x01) != 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xec: //If the P/V flag is set
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x04) != 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }

        case 0xfc: //If the sign flag is set
        
            {
                uint16_t nn = z80_mem_read(cpu.reg_PC) | (z80_mem_read(cpu.reg_PC + 1) << 8);
                cpu.reg_PC += 2; // Move PC past the nn bytes

                if ((cpu.Flags & 0x80) != 0) 
                {  
                    cpu.reg_SP = cpu.reg_PC + 3;
                    cpu.reg_PC = nn;

                    cpu.cycleCnt += 17; // 
                }

                else 
                {
                    cpu.cycleCnt += 10; 
                }
                break;
            }


        case 0xe9: // jp (hl): adds contence of HL into PC
            cpu.reg_PC = (((cpu.regH << 8) | cpu.regL));
            cpu.cycleCnt+=4;
            break;


        case 0xcd: //unconditional jump call
            push(cpu.reg_PC+2); //Insturction following this one pushed onto stack
            cpu.reg_PC = z80_mem_read16(cpu.reg_PC); //jump to nn
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
            incR(); //ED increases R by 2, one is already done by while loop
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
                //CARRY IS PRESERVED INSTRUCTIONS-----------------------------------------------------------
            {
                case 0xa1: //CPI - A - (HL), HL ← HL +1, BC ← BC – 1
                {  
                    uint8_t diff = cpu.regA - z80_mem_read((cpu.regH << 8) | cpu.regL);
                    uint8_t n = diff - ((cpu.Flags &0x10)>>4); // n = A-(HL)-HF
                    //cout << bitset<8>(cpu.regA) << " -\n" << bitset<8>( z80_mem_read((cpu.regH << 8) | cpu.regL)) << " -\n" <<bitset<8>((cpu.Flags &0x10)>>4) << " =\n" << bitset<8>(n) << endl;

                    cpu.regH = (((cpu.regH<<8)|cpu.regL) + 1) >> 8;//Hl++
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) + 1) & 0xff;//hL++
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--  

                    cpu.Flags = ZStable[diff] | (cpu.Flags & 0x01); // Z,S set; Carry unaffected
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a-(hl old) bit 1
                    ((cpu.regA & 0x0f) < ((diff & 0x0f) + ((cpu.Flags &0x10)>>4)))? (cpu.Flags |= 0x10):(cpu.Flags &= ~0x10); //Half Carry
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a-(hl old) bit 3
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags |= 0x02; //N Set

                    cpu.cycleCnt += 16;
                    break;
                }
            
                case 0xb1: //CPIR - A - (HL), HL ← HL +1, BC ← BC – 1
                {  
                    cout <<"RENTERING" << endl;
                    uint8_t diff = cpu.regA - z80_mem_read((cpu.regH << 8) | cpu.regL); //A - (HL)
                    uint8_t n = diff - ((cpu.Flags &0x10)>>4); // n = A-(HL)-HF

                    cpu.regH = (((cpu.regH<<8)|cpu.regL) + 1) >> 8;//Hl++
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) + 1) & 0xff;//hL++
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--  
                    cpu.Flags = ZStable[diff] | (cpu.Flags & 0x01); // Z,S set; Carry unaffected
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a-(hl old) bit 1
                    ((cpu.regA & 0x0f) < ((z80_mem_read((cpu.regH << 8) | cpu.regL) & 0x0f)))? (cpu.Flags |= 0x10):(cpu.Flags &= ~0x10); //Half Carry
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a-(hl old) bit 3
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags |= 0x02; //N Set

                    //If BC is not 0 and A ≠ (HL), the program counter is decremented by two and the instruction is repeated
                    if((bool(cpu.regB|cpu.regC) && (diff != 0))) //While BC != 0 AND A != (HL))
                    {   
                        cpu.reg_PC -= 2;
                        cpu.cycleCnt += 21;
                        break;
                    }

                    cpu.cycleCnt += 16;
                    break;
                }

                case 0xa9: //CPD - A – (HL), HL ← HL – 1, BC ← BC – 1
                {
                    uint8_t diff = cpu.regA - z80_mem_read((cpu.regH << 8) | cpu.regL);
                    uint8_t n = diff - ((cpu.Flags &0x10)>>4); // n = A-(HL)-HF
                    //cout << bitset<8>(cpu.regA) << " -\n" << bitset<8>( z80_mem_read((cpu.regH << 8) | cpu.regL)) << " -\n" <<bitset<8>((cpu.Flags &0x10)>>4) << " =\n" << bitset<8>(n) << endl;

                    cpu.regH = (((cpu.regH<<8)|cpu.regL) - 1) >> 8;//Hl--
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) - 1) & 0xff;//hL--
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--  

                    cpu.Flags = ZStable[diff] | (cpu.Flags & 0x01); // Z,S set; Carry unaffected
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a-(hl old) bit 1
                    ((cpu.regA & 0x0f) < ((diff & 0x0f) + ((cpu.Flags &0x10)>>4)))? (cpu.Flags |= 0x10):(cpu.Flags &= ~0x10); //Half Carry
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a-(hl old) bit 3
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags |= 0x02; //N Set

                    cpu.cycleCnt += 16;
                    break;
                }
            
                case 0xb9: //CPDR - A – (HL), HL ← HL – 1, BC ← BC – 1
                {
                    uint8_t diff = cpu.regA - z80_mem_read((cpu.regH << 8) | cpu.regL);
                    uint8_t n = diff - ((cpu.Flags &0x10)>>4); // n = A-(HL)-HF
                    //cout << bitset<8>(cpu.regA) << " -\n" << bitset<8>( z80_mem_read((cpu.regH << 8) | cpu.regL)) << " -\n" <<bitset<8>((cpu.Flags &0x10)>>4) << " =\n" << bitset<8>(n) << endl;

                    cpu.regH = (((cpu.regH<<8)|cpu.regL) - 1) >> 8;//Hl--
                    cpu.regL = (((cpu.regH<<8)|cpu.regL) - 1) & 0xff;//hL--
                    cpu.regB = (((cpu.regB<<8)|cpu.regC) - 1) >> 8;//Bc--
                    cpu.regC = (((cpu.regB<<8)|cpu.regC) - 1) & 0xff;//bC--  

                    cpu.Flags = ZStable[diff] | (cpu.Flags & 0x01); // Z,S set; Carry unaffected
                    (n & 0x01)? (cpu.Flags |= 0x20):(cpu.Flags &= ~0x20);//YF based on a-(hl old) bit 1
                    ((cpu.regA & 0x0f) < ((diff & 0x0f) + ((cpu.Flags &0x10)>>4)))? (cpu.Flags |= 0x10):(cpu.Flags &= ~0x10); //Half Carry
                    (n & 0x08)? (cpu.Flags |= 0x08):(cpu.Flags &= ~0x08);//XF based on a-(hl old) bit 3
                    (bool(cpu.regB | cpu.regC)) ? (cpu.Flags |= 0x04):(cpu.Flags &= ~0x04);//P/V is reset if BC == 0, set otherwise 
                    cpu.Flags |= 0x02; //N Set

                    //If BC is not 0 and A ≠ (HL), the program counter is decremented by two and the instruction is repeated
                    if((bool(cpu.regB|cpu.regC) && (diff != 0))) //While BC != 0 AND A != (HL))
                    {   
                        cpu.reg_PC -= 2;
                        cpu.cycleCnt += 21;
                        break;
                    }

                    cpu.cycleCnt += 16;
                    break;
                }

            }

                //SUBTRACTION W CARRY INSTRUCTIONS---------------------------------------------------------
            {
                case 0x42: //SUB W CARRY - Hl -= BC - CF
                {
                    uint16_t pair = sbc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regB<<8)|cpu.regC);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }

                case 0x52: //SUB W CARRY - Hl -= DE - CF
                {
                    uint16_t pair = sbc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regD<<8)|cpu.regE);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }

                case 0x62: //SUB W CARRY - Hl -= BC - CF
                {
                    uint16_t pair = sbc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regH<<8)|cpu.regL);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }   

                case 0x72: //SUB W CARRY - Hl -= SP - CF
                {
                    uint16_t pair = sbc16Flags((cpu.regH<<8)|cpu.regL,cpu.reg_SP);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }
            }

                //ADDITION W CARRY INSTRUCTIONS---------------------------------------------------------
            {
                case 0x4a: //ADD W CARRY - HL += BC + CF
                {
                    uint16_t pair = adc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regB<<8)|cpu.regC);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }

                case 0x5a: //ADD W CARRY - HL += DE + CF
                {
                    uint16_t pair = adc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regD<<8)|cpu.regE);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }

                case 0x6a: //ADD W CARRY - HL += HL + CF
                {
                    uint16_t pair = adc16Flags((cpu.regH<<8)|cpu.regL,(cpu.regH<<8)|cpu.regL);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }

                case 0x7a: //ADD W CARRY - HL += SP + CF
                {
                    uint16_t pair = adc16Flags((cpu.regH<<8)|cpu.regL,cpu.reg_SP);
                    cpu.regH = pair >> 8;
                    cpu.regL = pair & 0xff;
                    cpu.cycleCnt += 15;
                    break;
                }
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








        //================================================================================================================================
        //DD INSTRUCTIONS=================================================================================================================
        //================================================================================================================================    
        case 0xdd:
        {   
            incR();
            uint8_t inst2 = z80_mem_read(cpu.reg_PC++);
            switch(inst2)
            {
                //============================================================================================================
                //DDCB TABLE==================================================================================================
                //============================================================================================================
                case 0xcb:
                {
                    incR();
                    uint16_t offsetIX = displ2(cpu.reg_IX, z80_mem_read(cpu.reg_PC)); // ix addr +d
                    uint8_t inst3 = z80_mem_read(++cpu.reg_PC);
                    switch(inst3)
                    {
                            // z80_mem_write(z80_mem_read(offsetIX), rlcFlags(z80_mem_read(offsetIX)))

                        case 0x00: // RLC b 
                            cpu.regB = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x01: // RLC c
                            cpu.regC = rlcFlags(z80_mem_read(offsetIX)); 
                            cpu.cycleCnt+=23;
                            break;

                        case 0x02: // RLC d
                            cpu.regD = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x03: // RLC e
                            cpu.regE = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x04: // RLC h
                            cpu.regH = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x05: // RLC l
                            cpu.regL = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x06: // RLC (hl)
                            z80_mem_write(z80_mem_read(offsetIX), rlcFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x07: // RLC a
                            cpu.regA = rlcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;







                        case 0x08: //RRC b
                            cpu.regB = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x09: //RRC c
                            cpu.regC = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0A: //RRC d
                            cpu.regD = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0B: //RRC e
                            cpu.regE = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0C: //RRC h
                            cpu.regH = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0D: //RRC l
                            cpu.regL = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0E: //RRC (hl)
                            z80_mem_write(z80_mem_read(offsetIX), rrcFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0F: //RRC a
                            cpu.regA = rrcFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;






                        case 0x10: // rl b

                            {
                            cpu.regB = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x11: // rl c

                            {
                            cpu.regC = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x12: // rl d
                        
                            {
                            cpu.regD = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x13: // rl e
                                            
                            {
                            cpu.regE = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }
                            
                            break; 

                        case 0x14: // rl h
                                            
                            {
                            cpu.regH = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x15: // rl l
                                            
                            {
                            cpu.regL = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x16: // rl (hl)
                                            
                            {
                            z80_mem_write(z80_mem_read(offsetIX), rlFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x17: // rl a
                                            
                            {
                            cpu.regA = rlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;









                        case 0x18: // rr b 

                            {
                            cpu.regB = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x19: // rr c
                        
                            {
                            cpu.regC = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1A: // rr d
                                            
                            {
                            cpu.regD = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1B: // rr e
                                            
                            {
                            cpu.regE = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1C: // rr h
                                            
                            {
                            cpu.regH = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1D: // rr l
                                            
                            {
                            cpu.regL = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1E: // rr (hl)
                                            
                            {
                            z80_mem_write(z80_mem_read(offsetIX), rrFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x1F: // rr a
                                            
                            {
                            cpu.regA = rrFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            }

                            break;







                        case 0x20: //sla b
                            cpu.regB = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x21: //sla c
                            cpu.regC = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x22: //sla d
                            cpu.regD = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x23: //sla e
                            cpu.regE = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x24: //sla h
                            cpu.regH = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x25: //sla l
                            cpu.regL = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x26: //sla (hl)
                            z80_mem_write(z80_mem_read(offsetIX), slaFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break; 

                        case 0x27: //sla a
                            cpu.regA = slaFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;






                        case 0x28: // sra b
                            cpu.regB = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x29: // sra c
                            cpu.regC = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2A: // sra d
                            cpu.regD = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2B: // sra e
                            cpu.regE = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2C: // sra h
                            cpu.regH = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2D: // sra l
                            cpu.regL = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2E: // sra (hl)
                            z80_mem_write(z80_mem_read(offsetIX), sraFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break;
    
                        case 0x2F: // sra a
                            cpu.regA = sraFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;







                        case 0x30: // sll b
                            cpu.regB = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x31: // sll c
                            cpu.regC = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x32: // sll d 
                            cpu.regD = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x33: // sll e 
                            cpu.regE = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x34: // sll h 
                            cpu.regH = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x35: // sll l 
                            cpu.regL = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x36: // sll (hl)
                            z80_mem_write(z80_mem_read(offsetIX), sllFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x37: // sll a
                            cpu.regA = sllFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;





                            
                        case 0x38: // srl b
                            cpu.regB = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x39: // srl c
                            cpu.regC = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3A: // srl d
                            cpu.regD = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3B: // srl e
                            cpu.regE = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3C: // srl h
                            cpu.regH = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3D: // srl l
                            cpu.regL = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3E: // srl (hl)
                            z80_mem_write(z80_mem_read(offsetIX), srlFlags(z80_mem_read(offsetIX)));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3F: // srl a
                            cpu.regA = srlFlags(z80_mem_read(offsetIX));
                            cpu.cycleCnt+=23;
                            break;
                            



    // bit test 



                        case 0x40: // bit 0, b
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x41: // bit 0, c
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x42: // bit 0, d
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x43: // bit 0, e
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x44: // bit 0, h
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x45: // bit 0, l
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x46: // bit 0, (hl)
                            (z80_mem_read(offsetIX) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x47: // bit 0, a
                            ((z80_mem_read(offsetIX) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x48: // bit 1, b
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x49: // bit 1, c
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4A: // bit 1, d
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4B: // bit 1, e
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4C: // bit 1, h
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4D: // bit 1, l
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4E: // bit 1, (hl)
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4F: // bit 1, a
                            ((z80_mem_read(offsetIX) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x50: // bit 2, b
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x51: // bit 2, c
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x52: // bit 2, d
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x53: // bit 2, e
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x54: // bit 2, h
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x55: // bit 2, l
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x56: // bit 2, (hl)
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x57: // bit 2, a
                            ((z80_mem_read(offsetIX) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x58: // bit 3, b
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x59: // bit 3, c
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5A: // bit 3, d
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5B: // bit 3, e
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5C: // bit 3, h
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5D: // bit 3, l
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5E: // bit 3, (hl)
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5F: // bit 3, a
                            ((z80_mem_read(offsetIX) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x60: // bit 4, b
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x61: // bit 4, c
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x62: // bit 4, d
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x63: // bit 4, e
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x64: // bit 4, h
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x65: // bit 4, l
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x66: // bit 4, (hl)
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;
    
                        case 0x67: // bit 4, a
                            ((z80_mem_read(offsetIX) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x68: // bit 5, b
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x69: // bit 5, c
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6A: // bit 5, d
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6B: // bit 5, e
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6C: // bit 5, h
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6D: // bit 5, l
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6E: // bit 5, (hl)
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6F: // bit 5, a
                            ((z80_mem_read(offsetIX) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x70: // bit 6, b
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x71: // bit 6, c
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x72: // bit 6, d
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x73: // bit 6, e
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x74: // bit 6, h
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x75: // bit 6, l
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x76: // bit 6, (hl)
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x77: // bit 6, a
                            ((z80_mem_read(offsetIX) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x78: // bit 7, b
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x79: // bit 7, c
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7A: // bit 7, d
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7B: // bit 7, e
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7C: // bit 7, h
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7D: // bit 7, l
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7E: // bit 7, (hl)
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7F: // bit 7, a
                            ((z80_mem_read(offsetIX) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;


    // reset 


                        case 0x80: // res 0, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x81: // res 0, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x82: // res 0, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x83: // res 0, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x84: // res 0, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x85: // res 0, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x86: // res 0, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x01);
                            cpu.cycleCnt+=23;
                            break;
    
                        case 0x87: // res 0, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x88: // res 1, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x89: // res 1, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8A: // res 1, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8B: // res 1, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8C: // res 1, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8D: // res 1, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8E: // res 1, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x02);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8F: // res 1, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x90: // res 2, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x91: // res 2, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x92: // res 2, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x93: // res 2, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x94: // res 2, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x95: // res 2, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x96: // res 2, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x04);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x97: // res 2, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x98: // res 3, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x99: // res 3, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9A: // res 3, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9B: // res 3, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9C: // res 3, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9D: // res 3, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9E: // res 3, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x08);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9F: // res 3, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xA0: // res 4, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA1: // res 4, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA2: // res 4, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA3: // res 4, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA4: // res 4, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA5: // res 4, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA6: // res 4, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x10);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA7: // res 4, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xA8: // res 5, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA9: // res 5, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAA: // res 5, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAB: // res 5, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAC: // res 5, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAD: // res 5, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAE: // res 5, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x20);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAF: // res 5, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xB0: // res 6, b
                            cpu.regB = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB1: // res 6, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB2: // res 6, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB3: // res 6, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB4: // res 6, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB5: // res 6, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB6: // res 6, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x40);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB7: // res 6, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xB8: // res 7, b
                            cpu.regB =  z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB9: // res 7, c
                            cpu.regC = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBA: // res 7, d
                            cpu.regD = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBB: // res 7, e
                            cpu.regE = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBC: // res 7, h
                            cpu.regH = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBD: // res 7, l
                            cpu.regL = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBE: // res 7, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) & ~0x80);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBF: // res 7, a
                            cpu.regA = z80_mem_read(offsetIX) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;


    // set 


                        case 0xC0:// set 0, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC1:// set 0, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC2:// set 0, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC3:// set 0, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC4:// set 0, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC5:// set 0, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC6: // set 0, hl
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x01);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC7:// set 0, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x01;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xC8: // set 1, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC9: // set 1, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCA: // set 1, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCB: // set 1, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCC: // set 1, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCD: // set 1, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCE: // set 1, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x02);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCF: // set 1, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x02;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xD0: // set 2, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD1: // set 2, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD2: // set 2, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD3: // set 2, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD4: // set 2, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD5: // set 2, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD6: // set 2, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x04);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD7: // set 2, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x04;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xD8: // set 3, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD9: // set 3, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDA: // set 3, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDB: // set 3, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDC: // set 3, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDD: // set 3, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDE: // set 3, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x08);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDF: // set 3, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x08;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xE0: // set 4, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE1: // set 4, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE2: // set 4, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE3: // set 4, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE4: // set 4, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE5: // set 4, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE6: // set 4, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x10);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE7: // set 4, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x10;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xE8: // set 5, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE9: // set 5, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEA: // set 5, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEB: // set 5, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEC: // set 5, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xED: // set 5, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEE: // set 5, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x20);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEF: // set 5, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x20;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xF0: // set 6, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF1: // set 6, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF2: // set 6, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF3: // set 6, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF4: // set 6, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF5: // set 6, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF6: // set 6, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x40);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF7: // set 6, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x40;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xF8: // set 7, b
                            cpu.regB = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF9: // set 7, c
                            cpu.regC = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFA: // set 7, d
                            cpu.regD = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFB: // set 7, e
                            cpu.regE = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFC: // set 7, h
                            cpu.regH = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFD: // set 7, l
                            cpu.regL = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFE: // set 7, (hl)
                            z80_mem_write(z80_mem_read(offsetIX) , z80_mem_read(offsetIX) | 0x80);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFF: // set 7, a
                            cpu.regA = z80_mem_read(offsetIX) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                    }
                    break;
                }

                //LOAD REGISTER INSTRUCTIONS======================================================================
            {
                case 0x40: //LOAD INSTUCTION - Load Register B with Register B
                    cpu.regB = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x41: //LOAD INSTUCTION - Load Register B with Register C
                    cpu.regB = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x42: //LOAD INSTUCTION - Load Register B with Register D
                    cpu.regB = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x43: //LOAD INSTUCTION - Load Register B with Register E
                    cpu.regB = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x44: //LOAD INSTUCTION - Load Register B with Register IX High Bytes
                    cpu.regB = cpu.reg_IX >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x45: //LOAD INSTUCTION - Load Register B with Register IX Low Bytes
                    cpu.regB = cpu.reg_IX & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x47: //LOAD INSTUCTION - Load Register B with Register A
                    cpu.regB = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x50: //LOAD INSTUCTION - Load Register D with Register B
                    cpu.regD = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x51: //LOAD INSTUCTION - Load Register D with Register C
                    cpu.regD = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x52: //LOAD INSTUCTION - Load Register D with Register D
                    cpu.regD = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x53: //LOAD INSTUCTION - Load Register D with Register E
                    cpu.regD = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x54: //LOAD INSTUCTION - Load Register D with Register IX High Bytes
                    cpu.regD = cpu.reg_IX >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x55: //LOAD INSTUCTION - Load Register D with Register IX Low Bytes
                    cpu.regD = cpu.reg_IX & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x57: //LOAD INSTUCTION - Load Register D with Register A
                    cpu.regD = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x48: //LOAD INSTUCTION - Load Register C with Register B
                    cpu.regC = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x49: //LOAD INSTUCTION - Load Register C with Register C
                    cpu.regC = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x4a: //LOAD INSTUCTION - Load Register C with Register D
                    cpu.regC = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4b: //LOAD INSTUCTION - Load Register C with Register E
                    cpu.regC = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4c: //LOAD INSTUCTION - Load Register C with Register IX High Bytes
                    cpu.regC = cpu.reg_IX >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4d: //LOAD INSTUCTION - Load Register C with Register IX Low Bytes
                    cpu.regC = cpu.reg_IX & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4f: //LOAD INSTUCTION - Load Register C with Register A
                    cpu.regC = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x58: //LOAD INSTUCTION - Load Register E with Register B
                    cpu.regE = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x59: //LOAD INSTUCTION - Load Register E with Register C
                    cpu.regE = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x5a: //LOAD INSTUCTION - Load Register E with Register D
                    cpu.regE = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5b: //LOAD INSTUCTION - Load Register E with Register E
                    cpu.regE = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5c: //LOAD INSTUCTION - Load Register E with Register IX High Bytes
                    cpu.regE = cpu.reg_IX >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5d: //LOAD INSTUCTION - Load Register E with Register IX Low Bytes
                    cpu.regE = cpu.reg_IX & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5f: //LOAD INSTUCTION - Load Register E with Register A
                    cpu.regE = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x78: //LOAD INSTUCTION - Load Register A with Register B
                    cpu.regA = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x79: //LOAD INSTUCTION - Load Register A with Register C
                    cpu.regA = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x7a: //LOAD INSTUCTION - Load Register A with Register D
                    cpu.regA = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7b: //LOAD INSTUCTION - Load Register A with Register E
                    cpu.regA = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7c: //LOAD INSTUCTION - Load Register A with Register IX High Bytes
                    cpu.regA = cpu.reg_IX >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7d: //LOAD INSTUCTION - Load Register A with Register IX Low Bytes
                    cpu.regA = cpu.reg_IX & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7f: //LOAD INSTUCTION - Load Register A with Register A
                    cpu.regA = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x60: //LOAD INSTUCTION - Load IX High Byte with Register B
                    cpu.reg_IX = (cpu.regB << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x61: //LOAD INSTUCTION - Load IX High Byte with Register C
                    cpu.reg_IX = (cpu.regC << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;

                case 0x62: //LOAD INSTUCTION - Load IX High Byte with Register D
                    cpu.reg_IX = (cpu.regD << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x63: //LOAD INSTUCTION - Load IX High Byte with Register E
                    cpu.reg_IX = (cpu.regE << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x64: //LOAD INSTUCTION - Load IX High Byte with Register IX High Bytes
                    //Do nothing
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x65: //LOAD INSTUCTION - Load IX High Byte with Register IX Low Bytes
                    cpu.reg_IX = ((cpu.reg_IX & 0xff) << 8)|(cpu.reg_IX & 0xff); //low Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x67: //LOAD INSTUCTION - Load IX High Byte with Register A
                    cpu.reg_IX = (cpu.regA << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x68: //LOAD INSTUCTION - Load IX Low Byte with Register B
                    cpu.reg_IX = ((cpu.reg_IX >>8) << 8)|(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x69: //LOAD INSTUCTION - Load IX Low Byte with Register C
                    cpu.reg_IX = ((cpu.reg_IX >>8) << 8)|(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6a: //LOAD INSTUCTION - Load IX Low Byte with Register D
                    cpu.reg_IX = ((cpu.reg_IX >>8) << 8)|(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6b: //LOAD INSTUCTION - Load IX Low Byte with Register E
                    cpu.reg_IX = (cpu.reg_IX >>8) <<8|(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6c: //LOAD INSTUCTION - Load IX Low Byte with Register IX high
                    cpu.reg_IX = ((cpu.reg_IX >> 8) << 8)|(cpu.reg_IX >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6d: //LOAD INSTUCTION - Load IX Low Byte with Register IX low
                    //Do nothing
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6f: //LOAD INSTUCTION - Load IX Low Byte with Register IX high
                    cpu.reg_IX = ((cpu.reg_IX >> 8) << 8)|(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0xf9: //LOAD INSTRUCTION - Load SP with IX
                    cpu.reg_SP = cpu.reg_IX;
                    cpu.cycleCnt += 10;
                    break;
                
                case 0x21: //LOAD INSTRUCTION - Load IX with nn
                    cpu.reg_IX = z80_mem_read16(cpu.reg_PC);
                    cpu.reg_PC += 2; //next instruction
                    cpu.cycleCnt += 14;
                    break;
                

                case 0x06: //LOAD INSTRUCTION - Load value at n into register B
                    cpu.regB = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
        
                case 0x0e: //LOAD INSTRUCTION - Load value at n into register C
                    cpu.regC = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x16: //LOAD INSTRUCTION - Load value at n into register D
                    cpu.regD = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x1e: //LOAD INSTRUCTION - Load value at n into register E
                    cpu.regE = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x3e: //LOAD INSTRUCTION - Load value at n into register A
                    cpu.regA = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x26: //LOAD INSTRUCTION - Load value at n into high byte of IX
                    cpu.reg_IX = (z80_mem_read(int(cpu.reg_PC++)) << 8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x2e: //LOAD INSTRUCTION - Load value at n into low byte of IX
                    cpu.reg_IX = ((cpu.reg_IX >>8) <<8) | (z80_mem_read(int(cpu.reg_PC++)));
                    cpu.cycleCnt += 11;
                    break;

                case 0x22: //LOAD INSTRUCTION - Load IX into memory (nn)
                    z80_mem_write16(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1)), cpu.reg_IX); 
                    cpu.cycleCnt += 20;
                    break;
                
                case 0x2a: //LOAD INSTRUCTION - Load IX with (nn)
                    cpu.reg_IX =(z80_mem_read16(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1))));
                    cpu.cycleCnt += 20;
                    break;
                
                case 0x70: //LOAD INSTRUCTION - Load (IX+d) with b
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regB);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x71: //LOAD INSTRUCTION - Load (IX+d) with c
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regC);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x72: //LOAD INSTRUCTION - Load (IX+d) with D
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regD);
                    cpu.cycleCnt += 19;
                    break;

                case 0x73: //LOAD INSTRUCTION - Load (IX+d) with E
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regE);
                    cpu.cycleCnt += 19;
                    break;

                case 0x74: //LOAD INSTRUCTION - Load (IX+d) with H
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regH);
                    cpu.cycleCnt += 19;
                    break;

                case 0x75: //LOAD INSTRUCTION - Load (IX+d) with L
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regL);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x36: //LOAD INSTRUCTION - Load (IX+d) with n
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(++cpu.reg_PC-2)))),z80_mem_read(++cpu.reg_PC));
                    cpu.cycleCnt += 19;
                    break;

                case 0x77: //LOAD INSTRUCTION - Load (IX+d) with A
                    z80_mem_write((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regA);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x46: //LOAD INSTRUCTION - Load B with (IX+d)
                    cpu.regB = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x56: //LOAD INSTRUCTION - Load D with (IX+d)
                    cpu.regD = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x66: //LOAD INSTRUCTION - Load H with (IX+d)
                    cpu.regH = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x4e: //LOAD INSTRUCTION - Load C with (IX+d)
                    cpu.regC = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x5e: //LOAD INSTRUCTION - Load E with (IX+d)
                    cpu.regE = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x6e: //LOAD INSTRUCTION - Load L with (IX+d)
                    cpu.regL = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x7e: //LOAD INSTRUCTION - Load A with (IX+d)
                    cpu.regA = z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;

            }                
                
                
                //ARTITHMETIC INSTRUCTIONS ======================================================================
            {
                // INCREMENT INST -------------------------------------------------
                case 0x04: //INCRAMENT INSTRUCTION - Adds 1 to Register B
                    cpu.regB = incFlags(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x14: //INCRAMENT INSTRUCTION - Adds 1 to Register D
                    cpu.regD = incFlags(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x24: //INCRAMENT INSTRUCTION - Adds 1 to high byte of IX
                    cpu.reg_IX = (incFlags(cpu.reg_IX >> 8)<<8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x0c: //INCRAMENT INSTRUCTION - Adds 1 to Register C
                    cpu.regC = incFlags(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x1c: //INCRAMENT INSTRUCTION - Adds 1 to Register E
                    cpu.regE = incFlags(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2c: //INCRAMENT INSTRUCTION - Adds 1 to low byte of IX
                    cpu.reg_IX = (cpu.reg_IX >> 8)<<8|(incFlags(cpu.reg_IX & 0xff));
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x3c: //INCRAMENT INSTRUCTION - Adds 1 to Register A
                    cpu.regA = incFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x23: //INCRAMENT INSTRUCTION - Adds 1 to IX
                    cpu.reg_IX++;
                    cpu.cycleCnt += 10;
                    break;

                case 0x34: //INCRAMENT INSTRUCTION - Adds 1 to (IX+d)
                    z80_mem_write(
                        (displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),
                        incFlags(z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC)))))));
                    cpu.cycleCnt += 23;
                    break;
                
                //DECREMENT INSTRUCTIONS------------------------------------------------------------------------------
                case 0x05: //DECREMENT INSTRUCTION - Subs 1 from Register B
                    cpu.regB = decFlags(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x15: //DECREMENT INSTRUCTION - Subs 1 from Register D
                    cpu.regD = decFlags(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x25: //DECREMENT INSTRUCTION - Subs 1 from high byte of IX
                    cpu.reg_IX = (decFlags(cpu.reg_IX >> 8)<<8)|(cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x0d: //DECREMENT INSTRUCTION - Subs 1 from Register C
                    cpu.regC = decFlags(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x1d: //DECREMENT INSTRUCTION - Subs 1 from Register E
                    cpu.regE = decFlags(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2d: //DECREMENT INSTRUCTION - Subs 1 from low byte of IX
                    cpu.reg_IX = (cpu.reg_IX >> 8)<<8|(decFlags(cpu.reg_IX & 0xff));
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x3d: //DECREMENT INSTRUCTION - Subs 1 from Register A
                    cpu.regA = decFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2b: //DECREMENT INSTRUCTION - Subs 1 from IX
                    cpu.reg_IX--;
                    cpu.cycleCnt += 10;
                    break;

                case 0x35: //DECREMENT INSTRUCTION - Subs 1 from (IX+d)
                    z80_mem_write(
                        (displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))),
                        decFlags(z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC)))))));
                    cpu.cycleCnt += 23;
                    break;
                
                //ADDITION-------------------------------------------------------------------------
                case 0x80: //ADD INSTRUCTION - RegA += RegB
                    cpu.regA = addFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x81: //ADD INSTRUCTION - RegA += RegC
                    cpu.regA = addFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x82: //ADD INSTRUCTION - RegA += RegD
                    cpu.regA = addFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x83: //ADD INSTRUCTION - RegA += RegE 
                    cpu.regA = addFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x84: //ADD INSTRUCTION - RegA += IX high
                    cpu.regA = addFlags(cpu.regA, cpu.reg_IX >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x85: //ADD INSTRUCTION - RegA += IX Low
                    cpu.regA = addFlags(cpu.regA, cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x86: //ADD INSTRUCTION - RegA += memory[IX+d]
                    cpu.regA = addFlags(cpu.regA,z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x87: //ADD INSTRUCTION - RegA += RegA
                    cpu.regA = addFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //SUBTRACTION ----------------------------------------------------------------
                case 0x90: //SUB INSTRUCTION - RegA -= RegB
                    cpu.regA = subFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x91: //SUB INSTRUCTION - RegA -= RegC
                    cpu.regA = subFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x92: //SUB INSTRUCTION - RegA -= RegD
                    cpu.regA = subFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x93: //SUB INSTRUCTION - RegA -= RegE 
                    cpu.regA = subFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x94: //SUB INSTRUCTION - RegA -= IX high
                    cpu.regA = subFlags(cpu.regA, cpu.reg_IX >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x95: //SUB INSTRUCTION - RegA -= IX Low
                    cpu.regA = subFlags(cpu.regA, cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x96: //SUB INSTRUCTION - RegA -= memory[IX+d]
                    cpu.regA = subFlags(cpu.regA,z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x97: //SUB INSTRUCTION - RegA -= RegA
                    cpu.regA = subFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //ADD WITH CARRY-----------------------------------------------------------------------------------------------
                case 0x88: //ADD W CARRY INSTRUCTION - RegA += RegB + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x89: //ADD W CARRY INSTRUCTION - RegA += RegC + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8a: //ADD W CARRY INSTRUCTION - RegA += RegD + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8b: //ADD W CARRY INSTRUCTION - RegA += RegE + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8c: //ADD W CARRY INSTRUCTION - RegA += RegIX High + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.reg_IX >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8d: //ADD W CARRY INSTRUCTION - RegA += RegIX low + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8e: //ADD W CARRY INSTRUCTION - RegA += (IX+d) + Carry
                    cpu.regA = adcFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x8f: //ADD W CARRY INSTRUCTION - RegA += RegA + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //SUB WITH CARRY-----------------------------------------------------------------------------------------------
                case 0x98: //SUB W CARRY INSTRUCTION - RegA -= RegB - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x99: //SUB W CARRY INSTRUCTION - RegA -= RegC - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9a: //SUB W CARRY INSTRUCTION - RegA -= RegD - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9b: //SUB W CARRY INSTRUCTION - RegA -= RegE - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9c: //SUB W CARRY INSTRUCTION - RegA -= RegIX High - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.reg_IX >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9d: //SUB W CARRY INSTRUCTION - RegA -= RegIX low - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.reg_IX & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9e: //SUB W CARRY INSTRUCTION - RegA -= (IX+d) - Carry
                    cpu.regA = sbcFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x9f: //SUB W CARRY INSTRUCTION - RegA -= RegA - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
            }  //arithmetic instruct

            //LOGICAL STUFF=============================================================================================================
            {
                //AND INSTRUCTIONS -----------------------------------------------
                case 0xa0: //bitwise and register A with B
                    cpu.regA = andFlags(cpu.regA, cpu.regB); //bitwise AND for reg a with B
                    cpu.cycleCnt += 8;
                    break;

                case 0xa1: //bitwise and register A with C
                    cpu.regA = andFlags(cpu.regA, cpu.regC); //bitwise AND for reg a with C
                    cpu.cycleCnt += 8;
                    break;

                case 0xa2: //bitwise and register A with D
                    cpu.regA = andFlags(cpu.regA, cpu.regD); //bitwise AND for reg a with D
                    cpu.cycleCnt += 8;
                    break;

                case 0xa3: //bitwise and register A with E
                    cpu.regA = andFlags(cpu.regA, cpu.regE); //bitwise AND for reg a with E
                    cpu.cycleCnt += 8;
                    break;

                case 0xa4: //bitwise and register A with IX high
                    cpu.regA = andFlags(cpu.regA, (cpu.reg_IX >> 8)); //bitwise AND for reg a with H 
                    cpu.cycleCnt += 8;
                    break;

                case 0xa5: //bitwise and register A with IX low
                    cpu.regA = andFlags(cpu.regA, (cpu.reg_IX & 0xff)); //bitwise AND for reg a with l 
                    cpu.cycleCnt += 8;
                    break;

                case 0xa6: //bitwise and register A with (IX+d)
                    cpu.regA = andFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))))); //bitwise AND
                    cpu.cycleCnt += 19;
                    break;

                case 0xa7: //bitwise and register A with A
                    cpu.regA = andFlags(cpu.regA, cpu.regA); //bitwise AND for reg a with A
                    cpu.cycleCnt += 8;
                    break;
                
                //OR INSTRUCTIONS--------------------------------------------------------------------------------------
                case 0xb0: // OR A with B 
                    cpu.regA = cpu.regA | cpu.regB; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb1: // OR A with C 
                    cpu.regA = cpu.regA | cpu.regC; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb2: // OR A with D 
                    cpu.regA = cpu.regA | cpu.regD; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb3: // OR A with E 
                    cpu.regA = cpu.regA | cpu.regE; //bitwise OR for reg a with e  
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb4: // OR A with IX high
                    cpu.regA = cpu.regA | (cpu.reg_IX >> 8); //bitwise OR for reg a with h 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb5: // OR A with IX low
                    cpu.regA = cpu.regA | (cpu.reg_IX & 0xff); //bitwise OR for reg a with l 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb6: // OR A with (IX+b) 
                    cpu.regA = cpu.regA | z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))); //bitwise OR for reg a with hl 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 19;
                    break;

                case 0xb7: // OR A with A 
                    cpu.regA = cpu.regA | cpu.regA; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //XOR INSTRUCTIONS--------------------------------------------------------------------------------------
                case 0xa8: // XOR A with B 
                    cpu.regA = cpu.regA ^ cpu.regB; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xa9: // XOR A with C 
                    cpu.regA = cpu.regA ^ cpu.regC; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xaa: // XOR A with D 
                    cpu.regA = cpu.regA ^ cpu.regD; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xab: // XOR A with E 
                    cpu.regA = cpu.regA ^ cpu.regE; //bitwise XOR for reg a with e  
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xac: // XOR A with IX high
                    cpu.regA = cpu.regA ^ (cpu.reg_IX >> 8); //bitwise XOR for reg a with h 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xad: // XOR A with IX low
                    cpu.regA = cpu.regA ^ (cpu.reg_IX & 0xff); //bitwise XOR for reg a with l 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xae: // XOR A with (IX+b) 
                    cpu.regA = cpu.regA ^ (z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++)))))); //bitwise XOR for reg a with hl 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 19;
                    break;

                case 0xaf: // XOR A with A 
                    cpu.regA = cpu.regA ^ cpu.regA; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                // SUBTRACT FLAGS CHANGE, RESGISTERS DONT-----------------------------------------------------
                case 0xb8: //cp B from A
                    subFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb9: //cp C from A
                    subFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;

                case 0xba: //cp D from A
                    subFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;

                case 0xbb: //cp E from A
                    subFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break; 

                case 0xbc: //cp IX High from A
                    subFlags(cpu.regA, (cpu.reg_IX >> 8));
                    cpu.cycleCnt += 8;
                    break;

                case 0xbd: //cp IX low from A
                    subFlags(cpu.regA, (cpu.reg_IX & 0xff));
                    cpu.cycleCnt += 8;
                    break;

                case 0xbe: //cp HL from A
                    subFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IX, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;

                case 0xbf: //cp A from A
                    subFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0xfe: //cp n from A
                    subFlags(cpu.regA, z80_mem_read(cpu.reg_PC++));
                    cpu.cycleCnt+=7;
                    break;



            } //logical instruct

            //OTHER=======================================================================
            {
                case 0xe1: //POP into IX
                    cpu.reg_IX = popS();
                    cpu.cycleCnt += 14;
                    break;
                
                case  0xe3: //EXCHANGE INSTRUCTION - swap (sp) with L and (sp+1) with H
                {
                    uint16_t temp = cpu.reg_IX;
                    cpu.reg_IX= (z80_mem_read(cpu.reg_SP+1)<<8)|(z80_mem_read(cpu.reg_SP));
                    z80_mem_write16(cpu.reg_SP, temp);
                    cpu.cycleCnt += 23;
                    break;
                }

                case 0xe5: //PUSH INSTRUCTION- add IX onto stack
                    z80_mem_write( --cpu.reg_SP, (cpu.reg_IX >> 8) );
                    z80_mem_write( --cpu.reg_SP, (cpu.reg_IX & 0xff) );
                    cpu.cycleCnt += 15;
                    break;
                
                case 0xe9: // JUMP (IX): adds contence of IX into PC
                    cpu.reg_PC = cpu.reg_IX;
                    cpu.cycleCnt+=8;
                    break;

            }//Other instructions
                
            //16 BIT ARITHMETIC=====================================================
            {
                case 0x09: // ADD IX += BC
                    cpu.reg_IX = add16Flags(cpu.regB, cpu.regC, cpu.reg_IX);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x19: // ADD IX += DE
                    cpu.reg_IX = add16Flags(cpu.regD, cpu.regE, cpu.reg_IX);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x29: // ADD IX += IX
                    cpu.reg_IX = add16Flags(cpu.reg_IX>>8, cpu.reg_IX&0xff, cpu.reg_IX);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x39: // ADD IX += sp
                    cpu.reg_IX = add16Flags(cpu.reg_SP >> 8, cpu.reg_SP & 0xff, cpu.reg_IX);
                    cpu.cycleCnt+=15;
                    break;

            }//16b math 


                default:
                    cout << "Unknown DD Instruction: " << hex << int(inst) << endl;
                    return 1;
                    break;
            }

            break;//DD BREAK
        }
            








    //============================================================================================================================================================================================================================================================================================================================================================================================================
    //FD INSTRUCTIONS =========================================================================================================================================================================================================================================================================================================
    //============================================================================================================================================================================================================================================================================================================================================================================================================
        
        case 0xfd:
        {   
            incR();
            uint8_t inst2 = z80_mem_read(cpu.reg_PC++);
            switch(inst2)
            {
                // if the cb table for iy is called
                case 0xcb:
                {
                    incR();
                    uint16_t offsetIY = displ2(cpu.reg_IY, z80_mem_read(cpu.reg_PC)); // iY addr +d
                    uint8_t inst3 = z80_mem_read(++cpu.reg_PC);
                    switch(inst3)
                    {
                            // z80_mem_write(z80_mem_read(offsetIY), rlcFlags(z80_mem_read(offsetIY)))

                        case 0x00: // RLC b 
                            cpu.regB = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x01: // RLC c
                            cpu.regC = rlcFlags(z80_mem_read(offsetIY)); 
                            cpu.cycleCnt+=23;
                            break;

                        case 0x02: // RLC d
                            cpu.regD = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x03: // RLC e
                            cpu.regE = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x04: // RLC h
                            cpu.regH = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x05: // RLC l
                            cpu.regL = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x06: // RLC (hl)
                            z80_mem_write(z80_mem_read(offsetIY), rlcFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x07: // RLC a
                            cpu.regA = rlcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;







                        case 0x08: //RRC b
                            cpu.regB = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x09: //RRC c
                            cpu.regC = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0A: //RRC d
                            cpu.regD = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0B: //RRC e
                            cpu.regE = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0C: //RRC h
                            cpu.regH = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0D: //RRC l
                            cpu.regL = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0E: //RRC (hl)
                            z80_mem_write(z80_mem_read(offsetIY), rrcFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x0F: //RRC a
                            cpu.regA = rrcFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;






                        case 0x10: // rl b

                            {
                            cpu.regB = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x11: // rl c

                            {
                            cpu.regC = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x12: // rl d
                        
                            {
                            cpu.regD = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x13: // rl e
                                            
                            {
                            cpu.regE = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }
                            
                            break; 

                        case 0x14: // rl h
                                            
                            {
                            cpu.regH = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x15: // rl l
                                            
                            {
                            cpu.regL = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x16: // rl (hl)
                                            
                            {
                            z80_mem_write(z80_mem_read(offsetIY), rlFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x17: // rl a
                                            
                            {
                            cpu.regA = rlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;









                        case 0x18: // rr b 

                            {
                            cpu.regB = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x19: // rr c
                        
                            {
                            cpu.regC = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1A: // rr d
                                            
                            {
                            cpu.regD = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1B: // rr e
                                            
                            {
                            cpu.regE = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1C: // rr h
                                            
                            {
                            cpu.regH = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1D: // rr l
                                            
                            {
                            cpu.regL = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;

                        case 0x1E: // rr (hl)
                                            
                            {
                            z80_mem_write(z80_mem_read(offsetIY), rrFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            }
                            
                            break;

                        case 0x1F: // rr a
                                            
                            {
                            cpu.regA = rrFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            }

                            break;







                        case 0x20: //sla b
                            cpu.regB = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x21: //sla c
                            cpu.regC = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x22: //sla d
                            cpu.regD = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x23: //sla e
                            cpu.regE = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x24: //sla h
                            cpu.regH = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x25: //sla l
                            cpu.regL = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x26: //sla (hl)
                            z80_mem_write(z80_mem_read(offsetIY), slaFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break; 

                        case 0x27: //sla a
                            cpu.regA = slaFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;






                        case 0x28: // sra b
                            cpu.regB = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x29: // sra c
                            cpu.regC = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2A: // sra d
                            cpu.regD = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2B: // sra e
                            cpu.regE = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2C: // sra h
                            cpu.regH = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2D: // sra l
                            cpu.regL = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;

                        case 0x2E: // sra (hl)
                            z80_mem_write(z80_mem_read(offsetIY), sraFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break;
    
                        case 0x2F: // sra a
                            cpu.regA = sraFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;







                        case 0x30: // sll b
                            cpu.regB = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x31: // sll c
                            cpu.regC = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x32: // sll d 
                            cpu.regD = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x33: // sll e 
                            cpu.regE = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x34: // sll h 
                            cpu.regH = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x35: // sll l 
                            cpu.regL = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x36: // sll (hl)
                            z80_mem_write(z80_mem_read(offsetIY), sllFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x37: // sll a
                            cpu.regA = sllFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;





                            
                        case 0x38: // srl b
                            cpu.regB = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x39: // srl c
                            cpu.regC = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3A: // srl d
                            cpu.regD = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3B: // srl e
                            cpu.regE = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3C: // srl h
                            cpu.regH = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3D: // srl l
                            cpu.regL = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3E: // srl (hl)
                            z80_mem_write(z80_mem_read(offsetIY), srlFlags(z80_mem_read(offsetIY)));
                            cpu.cycleCnt+=23;
                            break;
                            
                        case 0x3F: // srl a
                            cpu.regA = srlFlags(z80_mem_read(offsetIY));
                            cpu.cycleCnt+=23;
                            break;
                            



    // bit test 



                        case 0x40: // bit 0, b
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x41: // bit 0, c
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x42: // bit 0, d
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x43: // bit 0, e
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x44: // bit 0, h
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x45: // bit 0, l
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x46: // bit 0, (hl)
                            (z80_mem_read(offsetIY) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x47: // bit 0, a
                            ((z80_mem_read(offsetIY) & 0x01) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x48: // bit 1, b
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x49: // bit 1, c
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4A: // bit 1, d
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4B: // bit 1, e
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4C: // bit 1, h
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4D: // bit 1, l
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4E: // bit 1, (hl)
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x4F: // bit 1, a
                            ((z80_mem_read(offsetIY) & 0x02) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x50: // bit 2, b
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x51: // bit 2, c
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x52: // bit 2, d
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x53: // bit 2, e
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x54: // bit 2, h
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x55: // bit 2, l
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x56: // bit 2, (hl)
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x57: // bit 2, a
                            ((z80_mem_read(offsetIY) & 0x04) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x58: // bit 3, b
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x59: // bit 3, c
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5A: // bit 3, d
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5B: // bit 3, e
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5C: // bit 3, h
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5D: // bit 3, l
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5E: // bit 3, (hl)
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x5F: // bit 3, a
                            ((z80_mem_read(offsetIY) & 0x08) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x60: // bit 4, b
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x61: // bit 4, c
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x62: // bit 4, d
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x63: // bit 4, e
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x64: // bit 4, h
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x65: // bit 4, l
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x66: // bit 4, (hl)
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;
    
                        case 0x67: // bit 4, a
                            ((z80_mem_read(offsetIY) & 0x10) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x68: // bit 5, b
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x69: // bit 5, c
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6A: // bit 5, d
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6B: // bit 5, e
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6C: // bit 5, h
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6D: // bit 5, l
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6E: // bit 5, (hl)
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x6F: // bit 5, a
                            ((z80_mem_read(offsetIY) & 0x20) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x70: // bit 6, b
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x71: // bit 6, c
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x72: // bit 6, d
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x73: // bit 6, e
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x74: // bit 6, h
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x75: // bit 6, l
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x76: // bit 6, (hl)
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x77: // bit 6, a
                            ((z80_mem_read(offsetIY) & 0x40) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;



                        case 0x78: // bit 7, b
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x79: // bit 7, c
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7A: // bit 7, d
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7B: // bit 7, e
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7C: // bit 7, h
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7D: // bit 7, l
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7E: // bit 7, (hl)
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;

                        case 0x7F: // bit 7, a
                            ((z80_mem_read(offsetIY) & 0x80) == 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40);
                            cpu.cycleCnt+=20;
                            break;


    // reset 


                        case 0x80: // res 0, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x81: // res 0, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x82: // res 0, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x83: // res 0, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x84: // res 0, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x85: // res 0, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x86: // res 0, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x01);
                            cpu.cycleCnt+=23;
                            break;
    
                        case 0x87: // res 0, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x01;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x88: // res 1, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x89: // res 1, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8A: // res 1, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8B: // res 1, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8C: // res 1, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8D: // res 1, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8E: // res 1, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x02);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x8F: // res 1, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x02;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x90: // res 2, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x91: // res 2, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x92: // res 2, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x93: // res 2, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x94: // res 2, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x95: // res 2, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x96: // res 2, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x04);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x97: // res 2, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x04;
                            cpu.cycleCnt+=23;
                            break;



                        case 0x98: // res 3, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x99: // res 3, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9A: // res 3, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9B: // res 3, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9C: // res 3, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9D: // res 3, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9E: // res 3, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x08);
                            cpu.cycleCnt+=23;
                            break;

                        case 0x9F: // res 3, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x08;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xA0: // res 4, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA1: // res 4, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA2: // res 4, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA3: // res 4, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA4: // res 4, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA5: // res 4, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA6: // res 4, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x10);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA7: // res 4, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x10;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xA8: // res 5, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xA9: // res 5, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAA: // res 5, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAB: // res 5, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAC: // res 5, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAD: // res 5, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAE: // res 5, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x20);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xAF: // res 5, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x20;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xB0: // res 6, b
                            cpu.regB = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB1: // res 6, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB2: // res 6, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB3: // res 6, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB4: // res 6, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB5: // res 6, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB6: // res 6, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x40);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB7: // res 6, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x40;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xB8: // res 7, b
                            cpu.regB =  z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xB9: // res 7, c
                            cpu.regC = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBA: // res 7, d
                            cpu.regD = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBB: // res 7, e
                            cpu.regE = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBC: // res 7, h
                            cpu.regH = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBD: // res 7, l
                            cpu.regL = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBE: // res 7, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) & ~0x80);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xBF: // res 7, a
                            cpu.regA = z80_mem_read(offsetIY) & ~0x80;
                            cpu.cycleCnt+=23;
                            break;


    // set 


                        case 0xC0:// set 0, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC1:// set 0, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC2:// set 0, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC3:// set 0, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC4:// set 0, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC5:// set 0, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC6: // set 0, hl
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x01);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC7:// set 0, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x01;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xC8: // set 1, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xC9: // set 1, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCA: // set 1, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCB: // set 1, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCC: // set 1, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCD: // set 1, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCE: // set 1, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x02);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xCF: // set 1, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x02;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xD0: // set 2, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD1: // set 2, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD2: // set 2, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD3: // set 2, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD4: // set 2, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD5: // set 2, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD6: // set 2, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x04);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD7: // set 2, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x04;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xD8: // set 3, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xD9: // set 3, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDA: // set 3, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDB: // set 3, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDC: // set 3, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDD: // set 3, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDE: // set 3, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x08);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xDF: // set 3, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x08;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xE0: // set 4, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE1: // set 4, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE2: // set 4, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE3: // set 4, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE4: // set 4, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE5: // set 4, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE6: // set 4, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x10);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE7: // set 4, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x10;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xE8: // set 5, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xE9: // set 5, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEA: // set 5, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEB: // set 5, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEC: // set 5, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xED: // set 5, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEE: // set 5, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x20);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xEF: // set 5, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x20;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xF0: // set 6, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF1: // set 6, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF2: // set 6, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF3: // set 6, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF4: // set 6, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF5: // set 6, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF6: // set 6, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x40);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF7: // set 6, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x40;
                            cpu.cycleCnt+=23;
                            break;



                        case 0xF8: // set 7, b
                            cpu.regB = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xF9: // set 7, c
                            cpu.regC = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFA: // set 7, d
                            cpu.regD = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFB: // set 7, e
                            cpu.regE = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFC: // set 7, h
                            cpu.regH = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFD: // set 7, l
                            cpu.regL = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFE: // set 7, (hl)
                            z80_mem_write(z80_mem_read(offsetIY) , z80_mem_read(offsetIY) | 0x80);
                            cpu.cycleCnt+=23;
                            break;

                        case 0xFF: // set 7, a
                            cpu.regA = z80_mem_read(offsetIY) | 0x80;
                            cpu.cycleCnt+=23;
                            break;

                    }
                    break;
                }

                //FD INSRTCUTIONS ======================================================================================================================
                //LOAD REGISTER INSTRUCTIONS======================================================================
            {
                case 0x40: //LOAD INSTUCTION - Load Register B with Register B
                    cpu.regB = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x41: //LOAD INSTUCTION - Load Register B with Register C
                    cpu.regB = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x42: //LOAD INSTUCTION - Load Register B with Register D
                    cpu.regB = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x43: //LOAD INSTUCTION - Load Register B with Register E
                    cpu.regB = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x44: //LOAD INSTUCTION - Load Register B with Register IX High Bytes
                    cpu.regB = cpu.reg_IY >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x45: //LOAD INSTUCTION - Load Register B with Register IX Low Bytes
                    cpu.regB = cpu.reg_IY & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x47: //LOAD INSTUCTION - Load Register B with Register A
                    cpu.regB = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x50: //LOAD INSTUCTION - Load Register D with Register B
                    cpu.regD = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x51: //LOAD INSTUCTION - Load Register D with Register C
                    cpu.regD = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x52: //LOAD INSTUCTION - Load Register D with Register D
                    cpu.regD = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x53: //LOAD INSTUCTION - Load Register D with Register E
                    cpu.regD = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x54: //LOAD INSTUCTION - Load Register D with Register IX High Bytes
                    cpu.regD = cpu.reg_IY >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x55: //LOAD INSTUCTION - Load Register D with Register IX Low Bytes
                    cpu.regD = cpu.reg_IY & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x57: //LOAD INSTUCTION - Load Register D with Register A
                    cpu.regD = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x48: //LOAD INSTUCTION - Load Register C with Register B
                    cpu.regC = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x49: //LOAD INSTUCTION - Load Register C with Register C
                    cpu.regC = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x4a: //LOAD INSTUCTION - Load Register C with Register D
                    cpu.regC = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4b: //LOAD INSTUCTION - Load Register C with Register E
                    cpu.regC = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4c: //LOAD INSTUCTION - Load Register C with Register IX High Bytes
                    cpu.regC = cpu.reg_IY >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4d: //LOAD INSTUCTION - Load Register C with Register IX Low Bytes
                    cpu.regC = cpu.reg_IY & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x4f: //LOAD INSTUCTION - Load Register C with Register A
                    cpu.regC = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x58: //LOAD INSTUCTION - Load Register E with Register B
                    cpu.regE = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x59: //LOAD INSTUCTION - Load Register E with Register C
                    cpu.regE = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x5a: //LOAD INSTUCTION - Load Register E with Register D
                    cpu.regE = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5b: //LOAD INSTUCTION - Load Register E with Register E
                    cpu.regE = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5c: //LOAD INSTUCTION - Load Register E with Register IX High Bytes
                    cpu.regE = cpu.reg_IY >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5d: //LOAD INSTUCTION - Load Register E with Register IX Low Bytes
                    cpu.regE = cpu.reg_IY & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x5f: //LOAD INSTUCTION - Load Register E with Register A
                    cpu.regE = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x78: //LOAD INSTUCTION - Load Register A with Register B
                    cpu.regA = cpu.regB;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x79: //LOAD INSTUCTION - Load Register A with Register C
                    cpu.regA = cpu.regC;
                    cpu.cycleCnt += 8;
                    break;

                case 0x7a: //LOAD INSTUCTION - Load Register A with Register D
                    cpu.regA = cpu.regD;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7b: //LOAD INSTUCTION - Load Register A with Register E
                    cpu.regA = cpu.regE;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7c: //LOAD INSTUCTION - Load Register A with Register IX High Bytes
                    cpu.regA = cpu.reg_IY >> 8; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7d: //LOAD INSTUCTION - Load Register A with Register IX Low Bytes
                    cpu.regA = cpu.reg_IY & 0xff; //High Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x7f: //LOAD INSTUCTION - Load Register A with Register A
                    cpu.regA = cpu.regA;
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x60: //LOAD INSTUCTION - Load IX High Byte with Register B
                    cpu.reg_IY = (cpu.regB << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x61: //LOAD INSTUCTION - Load IX High Byte with Register C
                    cpu.reg_IY = (cpu.regC << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;

                case 0x62: //LOAD INSTUCTION - Load IX High Byte with Register D
                    cpu.reg_IY = (cpu.regD << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x63: //LOAD INSTUCTION - Load IX High Byte with Register E
                    cpu.reg_IY = (cpu.regE << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x64: //LOAD INSTUCTION - Load IX High Byte with Register IX High Bytes
                    //Do nothing
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x65: //LOAD INSTUCTION - Load IX High Byte with Register IX Low Bytes
                    cpu.reg_IY = ((cpu.reg_IY & 0xff) << 8)|(cpu.reg_IY & 0xff); //low Byte
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x67: //LOAD INSTUCTION - Load IX High Byte with Register A
                    cpu.reg_IY = (cpu.regA << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x68: //LOAD INSTUCTION - Load IX Low Byte with Register B
                    cpu.reg_IY = ((cpu.reg_IY >>8) << 8)|(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x69: //LOAD INSTUCTION - Load IX Low Byte with Register C
                    cpu.reg_IY = ((cpu.reg_IY >>8) << 8)|(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6a: //LOAD INSTUCTION - Load IX Low Byte with Register D
                    cpu.reg_IY = ((cpu.reg_IY >>8) << 8)|(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6b: //LOAD INSTUCTION - Load IX Low Byte with Register E
                    cpu.reg_IY = (cpu.reg_IY >>8) <<8|(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6c: //LOAD INSTUCTION - Load IX Low Byte with Register IX high
                    cpu.reg_IY = ((cpu.reg_IY >> 8) << 8)|(cpu.reg_IY >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6d: //LOAD INSTUCTION - Load IX Low Byte with Register IX low
                    //Do nothing
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x6f: //LOAD INSTUCTION - Load IY Low Byte with Register IY high
                    cpu.reg_IY = ((cpu.reg_IY >> 8) << 8)|(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0xf9: //LOAD INSTRUCTION - Load SP with IY
                    cpu.reg_SP = cpu.reg_IY;
                    cpu.cycleCnt += 10;
                    break;
                
                case 0x21: //LOAD INSTRUCTION - Load IY with nn
                {
                    cpu.reg_IY = z80_mem_read16(cpu.reg_PC);
                    cpu.reg_PC += 2; //next instruction
                    cpu.cycleCnt += 14;
                    break;
                }

                case 0x06: //LOAD INSTRUCTION - Load value at n into register B
                    cpu.regB = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
        
                case 0x0e: //LOAD INSTRUCTION - Load value at n into register C
                    cpu.regC = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x16: //LOAD INSTRUCTION - Load value at n into register D
                    cpu.regD = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x1e: //LOAD INSTRUCTION - Load value at n into register E
                    cpu.regE = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x3e: //LOAD INSTRUCTION - Load value at n into register A
                    cpu.regA = z80_mem_read(int(cpu.reg_PC++));
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x26: //LOAD INSTRUCTION - Load value at n into high byte of IX
                    cpu.reg_IY = (z80_mem_read(int(cpu.reg_PC++)) << 8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 11;
                    break;
                
                case 0x2e: //LOAD INSTRUCTION - Load value at n into low byte of IX
                    cpu.reg_IY = ((cpu.reg_IY >>8) <<8) | (z80_mem_read(int(cpu.reg_PC++)));
                    cpu.cycleCnt += 11;
                    break;

                case 0x22: //LOAD INSTRUCTION - Load IX into memory (nn)
                    z80_mem_write16(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1)), cpu.reg_IY); 
                    cpu.cycleCnt += 20;
                    break;
                
                case 0x2a: //LOAD INSTRUCTION - Load IX with (nn)
                    cpu.reg_IY =(z80_mem_read16(((z80_mem_read(++cpu.reg_PC - 1)<<8)|z80_mem_read(++cpu.reg_PC - 1))));
                    cpu.cycleCnt += 20;
                    break;
                
                case 0x70: //LOAD INSTRUCTION - Load (IX+d) with b
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regB);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x71: //LOAD INSTRUCTION - Load (IX+d) with c
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regC);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x72: //LOAD INSTRUCTION - Load (IX+d) with D
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regD);
                    cpu.cycleCnt += 19;
                    break;

                case 0x73: //LOAD INSTRUCTION - Load (IX+d) with E
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regE);
                    cpu.cycleCnt += 19;
                    break;

                case 0x74: //LOAD INSTRUCTION - Load (IX+d) with H
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regH);
                    cpu.cycleCnt += 19;
                    break;

                case 0x75: //LOAD INSTRUCTION - Load (IX+d) with L
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regL);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x36: //LOAD INSTRUCTION - Load (IX+d) with n
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(++cpu.reg_PC-2)))),z80_mem_read(++cpu.reg_PC));
                    cpu.cycleCnt += 19;
                    break;

                case 0x77: //LOAD INSTRUCTION - Load (IX+d) with A
                    z80_mem_write((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),cpu.regA);
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x46: //LOAD INSTRUCTION - Load B with (IX+d)
                    cpu.regB = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x56: //LOAD INSTRUCTION - Load D with (IX+d)
                    cpu.regD = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x66: //LOAD INSTRUCTION - Load H with (IX+d)
                    cpu.regH = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x4e: //LOAD INSTRUCTION - Load C with (IX+d)
                    cpu.regC = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x5e: //LOAD INSTRUCTION - Load E with (IX+d)
                    cpu.regE = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x6e: //LOAD INSTRUCTION - Load L with (IX+d)
                    cpu.regL = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x7e: //LOAD INSTRUCTION - Load A with (IX+d)
                    cpu.regA = z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))));
                    cpu.cycleCnt += 19;
                    break;

            }                
                
                
                //ARTITHMETIC INSTRUCTIONS ======================================================================
            {
                // INCREMENT INST -------------------------------------------------
                case 0x04: //INCRAMENT INSTRUCTION - Adds 1 to Register B
                    cpu.regB = incFlags(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x14: //INCRAMENT INSTRUCTION - Adds 1 to Register D
                    cpu.regD = incFlags(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x24: //INCRAMENT INSTRUCTION - Adds 1 to high byte of IX
                    cpu.reg_IY = (incFlags(cpu.reg_IY >> 8)<<8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x0c: //INCRAMENT INSTRUCTION - Adds 1 to Register C
                    cpu.regC = incFlags(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x1c: //INCRAMENT INSTRUCTION - Adds 1 to Register E
                    cpu.regE = incFlags(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2c: //INCRAMENT INSTRUCTION - Adds 1 to low byte of IX
                    cpu.reg_IY = (cpu.reg_IY >> 8)<<8|(incFlags(cpu.reg_IY & 0xff));
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x3c: //INCRAMENT INSTRUCTION - Adds 1 to Register A
                    cpu.regA = incFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x23: //INCRAMENT INSTRUCTION - Adds 1 to IX
                    cpu.reg_IY++;
                    cpu.cycleCnt += 10;
                    break;

                case 0x34: //INCRAMENT INSTRUCTION - Adds 1 to (IX+d)
                    z80_mem_write(
                        (displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),
                        incFlags(z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC)))))));
                    cpu.cycleCnt += 23;
                    break;
                
                //DECREMENT INSTRUCTIONS------------------------------------------------------------------------------
                case 0x05: //DECREMENT INSTRUCTION - Subs 1 from Register B
                    cpu.regB = decFlags(cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x15: //DECREMENT INSTRUCTION - Subs 1 from Register D
                    cpu.regD = decFlags(cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x25: //DECREMENT INSTRUCTION - Subs 1 from high byte of IX
                    cpu.reg_IY = (decFlags(cpu.reg_IY >> 8)<<8)|(cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x0d: //DECREMENT INSTRUCTION - Subs 1 from Register C
                    cpu.regC = decFlags(cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x1d: //DECREMENT INSTRUCTION - Subs 1 from Register E
                    cpu.regE = decFlags(cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2d: //DECREMENT INSTRUCTION - Subs 1 from low byte of IX
                    cpu.reg_IY = (cpu.reg_IY >> 8)<<8|(decFlags(cpu.reg_IY & 0xff));
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x3d: //DECREMENT INSTRUCTION - Subs 1 from Register A
                    cpu.regA = decFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x2b: //DECREMENT INSTRUCTION - Subs 1 from IX
                    cpu.reg_IY--;
                    cpu.cycleCnt += 10;
                    break;

                case 0x35: //DECREMENT INSTRUCTION - Subs 1 from (IX+d)
                    z80_mem_write(
                        (displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))),
                        decFlags(z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC)))))));
                    cpu.cycleCnt += 23;
                    break;
                
                //ADDITION-------------------------------------------------------------------------
                case 0x80: //ADD INSTRUCTION - RegA += RegB
                    cpu.regA = addFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x81: //ADD INSTRUCTION - RegA += RegC
                    cpu.regA = addFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x82: //ADD INSTRUCTION - RegA += RegD
                    cpu.regA = addFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x83: //ADD INSTRUCTION - RegA += RegE 
                    cpu.regA = addFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x84: //ADD INSTRUCTION - RegA += IX high
                    cpu.regA = addFlags(cpu.regA, cpu.reg_IY >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x85: //ADD INSTRUCTION - RegA += IX Low
                    cpu.regA = addFlags(cpu.regA, cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x86: //ADD INSTRUCTION - RegA += memory[IX+d]
                    cpu.regA = addFlags(cpu.regA,z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x87: //ADD INSTRUCTION - RegA += RegA
                    cpu.regA = addFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //SUBTRACTION ----------------------------------------------------------------
                case 0x90: //SUB INSTRUCTION - RegA -= RegB
                    cpu.regA = subFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0x91: //SUB INSTRUCTION - RegA -= RegC
                    cpu.regA = subFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x92: //SUB INSTRUCTION - RegA -= RegD
                    cpu.regA = subFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x93: //SUB INSTRUCTION - RegA -= RegE 
                    cpu.regA = subFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x94: //SUB INSTRUCTION - RegA -= IX high
                    cpu.regA = subFlags(cpu.regA, cpu.reg_IY >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x95: //SUB INSTRUCTION - RegA -= IX Low
                    cpu.regA = subFlags(cpu.regA, cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x96: //SUB INSTRUCTION - RegA -= memory[IX+d]
                    cpu.regA = subFlags(cpu.regA,z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x97: //SUB INSTRUCTION - RegA -= RegA
                    cpu.regA = subFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //ADD WITH CARRY-----------------------------------------------------------------------------------------------
                case 0x88: //ADD W CARRY INSTRUCTION - RegA += RegB + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x89: //ADD W CARRY INSTRUCTION - RegA += RegC + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8a: //ADD W CARRY INSTRUCTION - RegA += RegD + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8b: //ADD W CARRY INSTRUCTION - RegA += RegE + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8c: //ADD W CARRY INSTRUCTION - RegA += RegIX High + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.reg_IY >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8d: //ADD W CARRY INSTRUCTION - RegA += RegIX low + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x8e: //ADD W CARRY INSTRUCTION - RegA += (IX+d) + Carry
                    cpu.regA = adcFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x8f: //ADD W CARRY INSTRUCTION - RegA += RegA + Carry
                    cpu.regA = adcFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //SUB WITH CARRY-----------------------------------------------------------------------------------------------
                case 0x98: //SUB W CARRY INSTRUCTION - RegA -= RegB - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x99: //SUB W CARRY INSTRUCTION - RegA -= RegC - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9a: //SUB W CARRY INSTRUCTION - RegA -= RegD - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9b: //SUB W CARRY INSTRUCTION - RegA -= RegE - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9c: //SUB W CARRY INSTRUCTION - RegA -= RegIX High - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.reg_IY >> 8);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9d: //SUB W CARRY INSTRUCTION - RegA -= RegIX low - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.reg_IY & 0xff);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0x9e: //SUB W CARRY INSTRUCTION - RegA -= (IX+d) - Carry
                    cpu.regA = sbcFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;
                
                case 0x9f: //SUB W CARRY INSTRUCTION - RegA -= RegA - Carry
                    cpu.regA = sbcFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
            }  //arithmetic instruct

            //LOGICAL STUFF=============================================================================================================
            {
                //AND INSTRUCTIONS -----------------------------------------------
                case 0xa0: //bitwise and register A with B
                    cpu.regA = andFlags(cpu.regA, cpu.regB); //bitwise AND for reg a with B
                    cpu.cycleCnt += 8;
                    break;

                case 0xa1: //bitwise and register A with C
                    cpu.regA = andFlags(cpu.regA, cpu.regC); //bitwise AND for reg a with C
                    cpu.cycleCnt += 8;
                    break;

                case 0xa2: //bitwise and register A with D
                    cpu.regA = andFlags(cpu.regA, cpu.regD); //bitwise AND for reg a with D
                    cpu.cycleCnt += 8;
                    break;

                case 0xa3: //bitwise and register A with E
                    cpu.regA = andFlags(cpu.regA, cpu.regE); //bitwise AND for reg a with E
                    cpu.cycleCnt += 8;
                    break;

                case 0xa4: //bitwise and register A with IX high
                    cpu.regA = andFlags(cpu.regA, (cpu.reg_IY >> 8)); //bitwise AND for reg a with H 
                    cpu.cycleCnt += 8;
                    break;

                case 0xa5: //bitwise and register A with IX low
                    cpu.regA = andFlags(cpu.regA, (cpu.reg_IY & 0xff)); //bitwise AND for reg a with l 
                    cpu.cycleCnt += 8;
                    break;

                case 0xa6: //bitwise and register A with (IX+d)
                    cpu.regA = andFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))))); //bitwise AND
                    cpu.cycleCnt += 19;
                    break;

                case 0xa7: //bitwise and register A with A
                    cpu.regA = andFlags(cpu.regA, cpu.regA); //bitwise AND for reg a with A
                    cpu.cycleCnt += 8;
                    break;
                
                //OR INSTRUCTIONS--------------------------------------------------------------------------------------
                case 0xb0: // OR A with B 
                    cpu.regA = cpu.regA | cpu.regB; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb1: // OR A with C 
                    cpu.regA = cpu.regA | cpu.regC; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb2: // OR A with D 
                    cpu.regA = cpu.regA | cpu.regD; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb3: // OR A with E 
                    cpu.regA = cpu.regA | cpu.regE; //bitwise OR for reg a with e  
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb4: // OR A with IX high
                    cpu.regA = cpu.regA | (cpu.reg_IY >> 8); //bitwise OR for reg a with h 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb5: // OR A with IX low
                    cpu.regA = cpu.regA | (cpu.reg_IY & 0xff); //bitwise OR for reg a with l 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb6: // OR A with (IX+b) 
                    cpu.regA = cpu.regA | z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))); //bitwise OR for reg a with hl 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 19;
                    break;

                case 0xb7: // OR A with A 
                    cpu.regA = cpu.regA | cpu.regA; //bitwise OR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                //XOR INSTRUCTIONS--------------------------------------------------------------------------------------
                case 0xa8: // XOR A with B 
                    cpu.regA = cpu.regA ^ cpu.regB; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xa9: // XOR A with C 
                    cpu.regA = cpu.regA ^ cpu.regC; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xaa: // XOR A with D 
                    cpu.regA = cpu.regA ^ cpu.regD; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xab: // XOR A with E 
                    cpu.regA = cpu.regA ^ cpu.regE; //bitwise XOR for reg a with e  
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xac: // XOR A with IX high
                    cpu.regA = cpu.regA ^ (cpu.reg_IY >> 8); //bitwise XOR for reg a with h 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xad: // XOR A with IX low
                    cpu.regA = cpu.regA ^ (cpu.reg_IY & 0xff); //bitwise XOR for reg a with l 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;

                case 0xae: // XOR A with (IX+b) 
                    cpu.regA = cpu.regA ^ (z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++)))))); //bitwise XOR for reg a with hl 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 19;
                    break;

                case 0xaf: // XOR A with A 
                    cpu.regA = cpu.regA ^ cpu.regA; //bitwise XOR for reg a with a 
                    xorFlags(cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                // SUBTRACT FLAGS CHANGE, RESGISTERS DONT-----------------------------------------------------
                case 0xb8: //cp B from A
                    subFlags(cpu.regA, cpu.regB);
                    cpu.cycleCnt += 8;
                    break;

                case 0xb9: //cp C from A
                    subFlags(cpu.regA, cpu.regC);
                    cpu.cycleCnt += 8;
                    break;

                case 0xba: //cp D from A
                    subFlags(cpu.regA, cpu.regD);
                    cpu.cycleCnt += 8;
                    break;

                case 0xbb: //cp E from A
                    subFlags(cpu.regA, cpu.regE);
                    cpu.cycleCnt += 8;
                    break; 

                case 0xbc: //cp IX High from A
                    subFlags(cpu.regA, (cpu.reg_IY >> 8));
                    cpu.cycleCnt += 8;
                    break;

                case 0xbd: //cp IX low from A
                    subFlags(cpu.regA, (cpu.reg_IY & 0xff));
                    cpu.cycleCnt += 8;
                    break;

                case 0xbe: //cp HL from A
                    subFlags(cpu.regA, z80_mem_read((displ2(cpu.reg_IY, int8_t(z80_mem_read(cpu.reg_PC++))))));
                    cpu.cycleCnt += 19;
                    break;

                case 0xbf: //cp A from A
                    subFlags(cpu.regA, cpu.regA);
                    cpu.cycleCnt += 8;
                    break;
                
                case 0xfe: //cp n from A
                    subFlags(cpu.regA, z80_mem_read(cpu.reg_PC++));
                    cpu.cycleCnt+=7;
                    break;



            } //logical instruct

            //OTHER=======================================================================
            {
                case 0xe1: //POP into IX
                    cpu.reg_IY = popS();
                    cpu.cycleCnt += 14;
                    break;
                
                case  0xe3: //EXCHANGE INSTRUCTION - swap (sp) with L and (sp+1) with H
                {
                    uint16_t temp = cpu.reg_IY;
                    cpu.reg_IY= (z80_mem_read(cpu.reg_SP+1)<<8)|(z80_mem_read(cpu.reg_SP));
                    z80_mem_write16(cpu.reg_SP, temp);
                    cpu.cycleCnt += 23;
                    break;
                }

                case 0xe5: //PUSH INSTRUCTION- add IX onto stack
                    z80_mem_write( --cpu.reg_SP, (cpu.reg_IY >> 8) );
                    z80_mem_write( --cpu.reg_SP, (cpu.reg_IY & 0xff) );
                    cpu.cycleCnt += 15;
                    break;
                
                case 0xe9: // JUMP (IX): adds contence of IX into PC
                    cpu.reg_PC = cpu.reg_IY;
                    cpu.cycleCnt+=8;
                    break;

            }//Other instructions
                
            //16 BIT ARITHMETIC=====================================================
            {
                case 0x09: // ADD IX += BC
                    cpu.reg_IY = add16Flags(cpu.regB, cpu.regC, cpu.reg_IY);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x19: // ADD IX += DE
                    cpu.reg_IY = add16Flags(cpu.regD, cpu.regE, cpu.reg_IY);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x29: // ADD IX += IX
                    cpu.reg_IY = add16Flags(cpu.reg_IY>>8, cpu.reg_IY&0xff, cpu.reg_IY);
                    cpu.cycleCnt+=15;
                    break;
                
                case 0x39: // ADD IX += sp
                    cpu.reg_IY = add16Flags(cpu.reg_SP >> 8, cpu.reg_SP & 0xff, cpu.reg_IY);
                    cpu.cycleCnt+=15;
                    break;

            }//16b math 

            




                //UNIDENTIFIED INSTRUCTION----------------------------------------------------------------------------------------------------------------------------------------------------------
                default:
                    cout << "Unknown FD Instruction: " << hex << int(inst) << endl;
                    printReg(cpu);
                    return 1;
                    break;
            }// end fd switch
            break;
    }//end fd case   

    

        //UNIDENTIFIED INSTRUCTION----------------------------------------------------------------------------------------------------------------------------------------------------------
        default:
            cout << "Unknown Instruction: " << hex << int(inst) << endl;
            printReg(cpu);
            return 1;
            break;

    }// end big switch 

    return 0;

}//end decode function 





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


uint8_t andFlags(uint8_t reg1, uint8_t reg2){

    uint8_t res = reg1 & reg2;

    cpu.Flags = ZSPXYtable[res]; //does S, Z, P , X , Y
    cpu.Flags |= 0x10; //Set H
    cpu.Flags &= ~0x02; // Reset N
    cpu.Flags &= ~0x01; // Reset C

    return res;

    /*            //flags (move this into a func that gets passed reg a) 
    cpu.Flags |= ~0x01; // sets the carry flag to 0 
    cpu.Flags |= ~0x02; // sets the sub flag 
    (__builtin_popcount(reg) % 2) ? (cpu.Flags |= ~0x04) : (cpu.Flags |= 0x04); // sets the parity flag
    cpu.Flags |= ~0x08;
    (reg = 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags |= ~0x40);
    (reg >= 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags |= ~0x80);*/

}

void xorFlags(uint8_t reg){

    cpu.Flags = ZSPXYtable[reg]; //does S, Z, P , X , Y
    cpu.Flags &= ~0x10; //Reset H
    cpu.Flags &= ~0x02; // Reset N
    cpu.Flags &= ~0x01; // Reset C


    /*
    cpu.Flags |= ~0x01; // sets the carry flag to 0 
    cpu.Flags |= ~0x02; // sets the sub flag 
    (__builtin_popcount(reg) % 2) ? (cpu.Flags |= ~0x04) : (cpu.Flags |= 0x04); // sets the parity flag
    cpu.Flags |= ~0x08;
    (reg = 0x00) ? (cpu.Flags |= 0x40) : (cpu.Flags |= ~0x40);
    (reg >= 0x80) ? (cpu.Flags |= 0x80) : (cpu.Flags |= ~0x80);
    */
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
    ((((reg1 & 0xf) + (reg2 & 0xf) + (cpu.Flags & 0x01)) & 0x10) == 0x10) ? (cpu.Flags |=  0x10) : (cpu.Flags &=  ~0x10); //FROM ROBM.DEV
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
    ((reg1 & 0xf) < ((reg2 & 0xf) + (cpu.Flags & 0x01))) ? (cpu.Flags |=  0x10) : (cpu.Flags &=  ~0x10); //FROM ROBM.DEV
    cpu.Flags |= ((diff >> 3) & 1) << 3; //3rd bit of result
    ((reg1 ^ reg2) & (diff ^ reg1) & 0x80) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04);; //If operants have same sign, but sub sign changes - overflow
    cpu.Flags |= 0x02; //N set
    ((reg2 + (cpu.Flags & 0x01))  > reg1) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //Carry if borrow needed
    
    return diff & 0xff;
}

//Push a value onto the stack
void push(uint16_t val)
{  
    cpu.reg_SP-=2;
    z80_mem_write16(cpu.reg_SP,val);

    /*
    printf("Pushed %04x\n", val);
    cout <<"STACK----------"<<endl;
    for(uint32_t i = cpu.reg_SP; i< 0x10000; i++)
        {printf("[%04x] = %02x\n", i, memory[i]);}
    */
}

//Pop off top of stack into pc
void retS()
{
    cpu.reg_PC = z80_mem_read16(cpu.reg_SP);
    cpu.reg_SP += 2;

    /*
    printf("Popped %04x\n", cpu.reg_PC);
    cout <<"STACK----------"<<endl;
    if(cpu.reg_SP != 0 )
    {
        for(uint32_t i = cpu.reg_SP; i< 0x10000 && i >0; i++)
        {printf("[%04x] = %02x\n", i, memory[i]);}
    }
    else    
        {cout << "EMPTY STACK\n" << endl;}
    */
    
}

//pops off top of stack into a register
uint16_t popS()
{
    uint16_t low = z80_mem_read(cpu.reg_SP++); 
    uint16_t high = z80_mem_read(cpu.reg_SP++); 
    return (high << 8) | low;
}

//subtracts registers and the carry bit if set
uint16_t sbc16Flags(uint16_t reg1, uint16_t reg2)
{
    uint16_t diff = reg1 - reg2 - (cpu.Flags & 0x01);
    //printf("x%04x - x%04x - x%04x = x%04x\n", reg1, reg2, (cpu.Flags & 0x01), diff);
    //cout << bitset<16>(reg1) << " -\n" << bitset<16>(reg2) << " -\n" << bitset<16>((cpu.Flags & 0x01)) << " =\n" << bitset<16>(diff) << endl;;
   

    (diff & 0x8000) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80); //Sign
    (diff == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40); //Z set if result is 0
    cpu.Flags |= ((diff >> 13) & 1) << 5; //13th bit of result
    ((reg1 & 0xfff) < ((reg2 & 0xfff) + (cpu.Flags & 0x01))) ? (cpu.Flags |=  0x10) : (cpu.Flags &=  ~0x10); //FROM ROBM.DEV, borrow from bit 12
    cpu.Flags |= ((diff >> 11) & 1) << 3; //11th bit of result
    (bool((reg1 ^ diff ^ reg2) & 0x8000)) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04);; //If operants have same sign, but sub sign changes - overflow
    cpu.Flags |= 0x02; //N set
    ((reg2 + (cpu.Flags & 0x01))  > reg1) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //Carry if borrow needed
    
    return diff & 0xffff;
}

uint16_t adc16Flags(uint16_t reg1, uint16_t reg2)
{
    uint32_t sum = reg1 + reg2 + (cpu.Flags & 0x01);
    printf("x%04x + x%04x + x%04x = x%04x\n", reg1, reg2, (cpu.Flags & 0x01), sum);
    cout << bitset<16>(reg1) << " +\n" << bitset<16>(reg2) << " +\n" << bitset<16>((cpu.Flags & 0x01)) << " =\n" << bitset<16>(sum) << endl;;
   

    (sum & 0x8000) ? (cpu.Flags |= 0x80) : (cpu.Flags &= ~0x80); //Sign
    (sum == 0) ? (cpu.Flags |= 0x40) : (cpu.Flags &= ~0x40); //Z set if result is 0
    cpu.Flags |= ((sum >> 13) & 1) << 5; //13th bit of result
    (0xfff < ((reg1 & 0xfff) + (reg2 & 0xfff) + (cpu.Flags & 0x01))) ? (cpu.Flags |=  0x10) : (cpu.Flags &=  ~0x10); //FROM ROBM.DEV, carry to bit 12
    cpu.Flags |= ((sum >> 11) & 1) << 3; //11th bit of result
    (bool((reg1 ^ sum ^ reg2) & 0x8000)) ? (cpu.Flags |= 0x04) : (cpu.Flags &= ~0x04);; //If operants have same sign, but sub sign changes - overflow
    cpu.Flags &= ~0x02; //N reset
    (sum > 0xffff) ? (cpu.Flags |= 0x01) : (cpu.Flags &= ~0x01); //Carry if sum more than 16 bits needed
    
    return sum & 0xffff;
}

void incR() //INCREASING R DOES NOT CHANGE THE 8th BIT
{
    uint8_t bit7 = cpu.reg_R & 0x80; //7th bit is stored
    cpu.reg_R++;
    (bit7)? (cpu.reg_R |= 0x80):(cpu.reg_R &= ~0x80);//8th bit restored
}

//Displaces a unsigned value by (signed) d.
uint16_t displ2(uint16_t val, uint8_t d)
{
    int8_t offset = (int8_t)d;
    return val + offset;
}







uint8_t sraFlags(uint8_t reg)
{
    uint8_t carry = reg & 0x01;                 //bit 0 coppied into carry flag 
    reg = reg&0x80 | reg >> 1;                  //Shift right by 1, preserve bit 7
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, C flags
                                                //H & N Flags reset

    return reg;
}

uint8_t slaFlags(uint8_t reg)
{
    uint8_t carry = 0x00;                       //inisitallized the carry flag as 0 for later
    if((reg&0x80) != 0x00){carry = 0x01;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg << 1;                             //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset     
    
    return reg;
}

uint8_t srlFlags(uint8_t reg)
{
    uint8_t carry = 0x00;                       //inisitallized the carry flag as 0 for later
    if((reg&0x01) != 0x00){carry = 0x01;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg >> 1;                  //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset     
    
    return reg;
}

uint8_t sllFlags(uint8_t reg)
{
    uint8_t carry = 0x00;                       //inisitallized the carry flag as 0 for later
    if((reg&0x08) != 0x00){carry = 0x01;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg << 1 | 0x01;                  //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset 

    return reg;
}








uint8_t rlcFlags(uint8_t reg)
{
    uint8_t carry = 0x00;                       //inisitallized the carry flag as 0 for later
    if((reg&0x80) != 0x00){carry = 0x01;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg << 1 | carry;                     //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset     
    return reg;
}

uint8_t rrcFlags(uint8_t reg)
{
    uint8_t bitSev = 0x00;
    uint8_t carry = reg&0x01;                       //inisitallized the carry flag as 0 for later
    if((reg&0x80) != 0x00){bitSev = 0x80;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg >> 1 | bitSev;                     //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset     
    return reg;
}

uint8_t rlFlags(uint8_t reg)
{
    uint8_t preCarry = cpu.Flags &= 0x01;       //sets a variable for the flags reg before stuff happens
    // cout << int(preCarry) << endl;
    uint8_t carry = 0x00;                       //inisitallized the carry flag as 0 for later
    if((reg&0x80) != 0x00){carry = 0x01;}       //if the reg's 7th bit is set then the carry bit is set 
    reg = reg << 1 | preCarry;                     //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;        //Does, S Z Y X P, Carry flags
                                                //H & N Flags reset     
    return reg;
}

uint8_t rrFlags(uint8_t reg)
{
    uint8_t preCarry = cpu.Flags&0x01;                            //sets a variable for the flags reg before stuff happens
    if (preCarry != 0x00) {preCarry = 0x80;}
    uint8_t carry = reg&0x01;                   
    reg = reg >> 1 | preCarry;                          //sets the register shifted to the right one bit with the carry flag
    cpu.Flags = ZSPXYtable[reg] | carry;                //Does, S Z Y X P, Carry flags
                                                        //H & N Flags reset     
    return reg;
}





void bitZero(uint8_t reg)
{
    if( (reg&0x01) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitOne(uint8_t reg)
{
    if( (reg&0x02) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitTwo(uint8_t reg)
{
    if( (reg&0x04) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitThree(uint8_t reg)
{
    if( (reg&0x08) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitFour(uint8_t reg)
{
    if( (reg&0x10) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitFive(uint8_t reg)
{
    if( (reg&0x20) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitSix(uint8_t reg)
{
    if( (reg&0x40) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}

void bitSeven(uint8_t reg)
{
    if( (reg&0x80) == 0x00) 
    {
        cpu.Flags|=0x40;
    }
    else
    {
        cpu.Flags&=~0x40;
    }
    cpu.Flags|=0x10;
    cpu.Flags&=~0x02;
}



//MAIN==============================================================================================================================
int main(){
    
    
    cout << "Max Castle is feeling festive" << endl; //File running check
    
    // NECESSSARY CODE
    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

   z80_mem_load(fileRun.c_str()); //Load into memory

   //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
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


//    cpu.regH = 0x11;
//    cpu.regL = 0x18;
//    cpu.regA = 0xf3;
//    cpu.regB = 0xfe;
//    cpu.regC = 0xff;
//    memory[0x1118] = 0x52;
//    memory[0x1117] = 0x00;
//    memory[0x1116] = 0xF3;
//    memory[0x2225] = 0xc5;
//    memory[0x2224] = 0x59;
//    memory[0x2223] = 0x66;
//    //cpu.Flags |= 0x01;




    // uint16_t start1 = 0x1004;
    // uint16_t start2 = start1 + 3;
   
   
    // cpu.reg_IX = 1000;
    // memory[start1] = 0x00;
    // memory[start1+3] = 0xb0;
    // cpu.regB = 0x02;

    
    
    // z80_mem_write(0x00, 0xdd); //dd
    // z80_mem_write(0x01, 0xcb); //cb
    // z80_mem_write(0x0, 0x06); //IX+d
    // z80_mem_write(0x03, 0x28); //
    // z80_mem_write(0x04, 0xdd); //dd
    // z80_mem_write(0x05, 0xcb); //cb
    // z80_mem_write(0x06, 0x03); //IX+d
    // z80_mem_write(0x07, 0x28); //
    // z80_mem_write(0x08, 0xdd); //dd
    // z80_mem_write(0x09, 0xcb); //cb
    // z80_mem_write(0x0a, 0x03); //IX+d
    // z80_mem_write(0x0b, 0x28); //
    // z80_mem_write(0x0c, 0x80); //a+=b
    // z80_mem_write(0x0d, 0x76); //halt








    // ======= TESTING DAA OP =========

    // z80_mem_write(0x00, 0x3E); // LD A, 0x15 (Load 0x15 into A)
    // z80_mem_write(0x01, 0x15); 
    // z80_mem_write(0x02, 0x06); // LD B, 0x27 (Load 0x27 into B)
    // z80_mem_write(0x03, 0x27); 
    // z80_mem_write(0x04, 0x80); // ADD A, B (Add A and B; result is 0x3C)
    // z80_mem_write(0x05, 0x27); // DAA (Adjust A to BCD; result should be 0x42)

    // z80_mem_write(0x06, 0x3E); // LD A, 0x99 (Load 0x99 into A)
    // z80_mem_write(0x07, 0x99); 
    // z80_mem_write(0x08, 0xC6); // ADD A, 0x01 (Add 0x01 to A; result is 0x9A)
    // z80_mem_write(0x09, 0x01); 
    // z80_mem_write(0x0A, 0x27); // DAA (Adjust A to BCD; result should be 0x00, CF set)
    // z80_mem_write(0x0B, 0x3E); // LD A, 0x45 (Load 0x45 into A)
    // z80_mem_write(0x0C, 0x45); 
    // z80_mem_write(0x0D, 0xD6); // SUB A, 0x25 (Subtract 0x25 from A; result is 0x20)
    // z80_mem_write(0x0E, 0x25); 
    // z80_mem_write(0x0F, 0x27); // DAA (Adjust A to BCD; result should be 0x20)
    // z80_mem_write(0x10, 0x76); // HALT

// =============================================================================================
    

// call functions test ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

   /*{ // max testing code

   printReg(cpu);

// Set up the test program in memory

// Load B with a nonzero value (e.g., 0x05)
z80_mem_write(0x00, 0x06); // LD B, n
z80_mem_write(0x01, 0x05); // n = 0x05

// DJNZ to a label (relative jump backwards by 2 bytes)
z80_mem_write(0x02, 0x10); // DJNZ d
z80_mem_write(0x03, 0xFE); // d = -2 (loop back to DJNZ)

// Halt the CPU after the loop to prevent further execution
z80_mem_write(0x04, 0x76); // HALT

// Expected Behavior:
// The DJNZ instruction decrements B and jumps back to itself until B reaches 0.
// The HALT instruction will only execute after DJNZ completes all iterations (5 in this case).

// Test Execution:
// Initialize your Z80 emulator, set the program counter to 0x00, and run the program.
// Verify that B starts at 0x05, decrements with each iteration, and stops at 0 after 5 loops.
// Ensure that the emulator halts at 0x04 as expected.


   }*/



// call functions test------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
  {
// Write the program into memory
z80_mem_write(0x0000, 0xC4); // CALL NZ, nn
z80_mem_write(0x0001, 0x34); // Low byte of address nn (0x1234)
z80_mem_write(0x0002, 0x12); // High byte of address nn (0x1234)
z80_mem_write(0x1234, 0x76); // HALT

// Initialize CPU state
cpu.reg_PC = 0x0000; // Start at 0x0000
cpu.reg_SP = 0xFFFE; // Stack pointer
cpu.Flags = 0x00;    // Zero flag unset


  }*/





    for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }



    z80_execute();

    cout << bitset<8>(memory[0x5000]) << endl;
    
    
//     for (int i =0; i < 3; i++)
//     {
//         printf("\tram[%04x] = %02x\t ", start1+i, memory[start1+i]);
//         printf("ram[%04x] = %02x\n ", start2+i, memory[start2+i]);
//     }

    return 0;
}