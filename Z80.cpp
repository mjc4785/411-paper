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
#include "registers.h"
using namespace std;

//FILE NAMES FOR RUNNING===================================================================================================
const string filenameEMMA = "C:\\Users\\ekcha\\OneDrive\\Documents\\GitHub\\411-paper\\load-regs.bin";
const string filenameMAX = "C:\\Users\\Maxwell\\OneDrive\\Desktop\\411-paper\\load-regs.bin"; //Max change this if you want to run it 
const string fileRun = filenameEMMA;

//DEFINING IMPORTNANT THINGS=======================================================================================================
int const CYCLES = 1024;
int const MEMSIZE = 65536;
static uint8_t memory[MEMSIZE];
Z80 cpu;


//FUNCTION DEFINITIONS=========================================
void decode();
void printReg(Z80);
void setflags(uint8_t, bool, bool, bool, bool);


//OUT OF SIGHT OUT OF MIND (DONT TOUCH THESE I DIDNT WRITE THEM)====================================================================
void z80_mem_write(uint16_t addr, uint8_t value) {
    memory[addr] = value;
}

uint8_t z80_mem_read(uint16_t addr) {
    return memory[addr];
}

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
void z80_execute(){

    while(cpu.cycleCnt < CYCLES)
    {
        decode();
        if(memory[int(cpu.reg_PC++)] == 0x76){break;} //Leave the loop if halting
    }
    
    // return CompletedCylces;
    return;
}

//Determine which instruction to run and execute it
void decode()
{
    switch(memory[int(cpu.reg_PC)])
    {
        //HALT------------------------------------------------------------------------
        case 0x76: //HALT INSTRUCTION - print out all the registers and dump memory to .bin file
            printReg(cpu);
            z80_mem_dump("memory.bin");
            break;
        
        //8-BIT GENERAL REGISTER LOADS------------------------------------------------
        case 0x3e: //LOAD INSTRUCTION - Load value at n into register A
            cpu.regA = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x06: //LOAD INSTRUCTION - Load value at n into register B
            cpu.regB = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x0e: //LOAD INSTRUCTION - Load value at n into register C
            cpu.regC = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x16: //LOAD INSTRUCTION - Load value at n into register D
            cpu.regD = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x1e: //LOAD INSTRUCTION - Load value at n into register E
            cpu.regE = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x26: //LOAD INSTRUCTION - Load value at n into register H
            cpu.regH = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x2e: //LOAD INSTRUCTION - Load value at n into register L
            cpu.regL = memory[int(++cpu.reg_PC)];
            cpu.cycleCnt += 7;
            break;
        
        //8-BIT ADDITION ARITHMETIC-------------------------------------------------
        case 0x80:
            uint8_t sum;// = cpu.regA + cpu.regB;
            bool halfCarry; //implement
            bool overflow; //implement
            bool carry; //implement
            setflags(sum,halfCarry, overflow, false, carry);

        //UNIDENTIFIED INSTRUCTION--------------------------------------------------
        default:
            cout << "Unknown Instruction" << endl;
            break;
    }
}

//OTHER FUNCTIONS=====================================================================================================================
//Print out all the registers in the cpu
void printReg(Z80 cpu)
{
    cout << "\n*=================Z80========================*" << endl;
    printf("regA: 0x%02X\n", cpu.regA) ;
    printf("Flags: 0x%02X\n", cpu.Flags);
    printf("regB: 0x%02X\n", cpu.regB) ;
    printf("regC: 0x%02X\n", cpu.regC) ;
    printf("regD: 0x%02X\n", cpu.regD) ;
    printf("regE: 0x%02X\n", cpu.regE) ;
    printf("regH: 0x%02X\n", cpu.regH) ;
    printf("regL: 0x%02X\n", cpu.regL) ;

    printf("reg_A: 0x%02X\n", cpu.reg_A);
    printf("reg_F: 0x%02X\n", cpu.reg_F);
    printf("reg_B: 0x%02X\n", cpu.reg_B);
    printf("reg_C: 0x%02X\n", cpu.reg_C);
    printf("reg_D: 0x%02X\n", cpu.reg_D);
    printf("reg_E: 0x%02X\n", cpu.reg_E);
    printf("reg_H: 0x%02X\n", cpu.reg_H);
    printf("reg_L: 0x%02X\n", cpu.reg_L);

    printf("reg_I: 0x%02X\n", cpu.reg_I);
    printf("reg_R: 0x%02X\n", cpu.reg_R);

    printf("reg_IX: 0x%04X\n", cpu.reg_IX);
    printf("reg_IY: 0x%04X\n", cpu.reg_IY);
    printf("reg_SP: 0x%04X\n", cpu.reg_SP);
    printf("reg_PC: 0x%04X\n", cpu.reg_PC);

    printf("Cycle Count: %d\n", cpu.cycleCnt);
    cout << "*============================================*\n" << endl;
}


//Edits the flag register according to the arithmetic opperation results - I DONT KNOW IF THIS WORKS YET
void setflags(uint8_t result, bool halfCarry, bool overflow, bool subtraction, bool carry)
{
    cpu.Flags = 0; //Reset the flags for each result

    if (result < 0)     {cpu.Flags = cpu.Flags || 0x10000000;}  //(bit 7) S-Flag  [0:(+/0)result | 1:(-)result]
    if (result == 0)    {cpu.Flags = cpu.Flags || 0x01000000;}  //(bit 6) Z-flag  [0: result!=0 | 1: result==0]
    //Bit 5 unused 
    if (halfCarry)      {cpu.Flags = cpu.Flags || 0x00010000;}  //(bit 4) H-Flag [0:carry absent | 1: carry present]
    //Bit 3 unused
    if(overflow)        {cpu.Flags = cpu.Flags || 0x00000100;}  //(bit 2) P/V-Flag [0: no overflow or odd number of 1 bits]
    if(subtraction)     {cpu.Flags = cpu.Flags || 0x00000010;}  //(bit 1) N-Flag [0:Addition | 1: Subtraction]
    if(carry)           {cpu.Flags = cpu.Flags || 0x00000001;}  //(bit 0) C-Flag [0: no carry/borrow | 1: carry/borrow]
}

//MAIN==============================================================================================================================
int main(){
    cout << "Max Castle is feeling up-really-cool-guy" << endl; //File running check

    z80_mem_load(fileRun.c_str()); //Load into memory
    for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }

    z80_execute();

    return 0;
}