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
const string fileRun = filenameMAX;

//DEFINING IMPORTNANT THINGS=======================================================================================================
int const CYCLES = 1024;
int const MEMSIZE = 65536;
static uint8_t memory[MEMSIZE];
Z80 cpu;


//FUNCTION DEFINITIONS=========================================
void decode();
void printReg(Z80);


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
void z80_execute(){

    while(cpu.cycleCnt < CYCLES)
    {
        decode();
        if(memory[int(cpu.reg_PC++)] == 0x76){break;} //Leave the loop if halting
    }
    
    // return CompletedCylces;
    return;
}

void decode()
{
    switch(memory[int(cpu.reg_PC)])
    {
        case 0x76: //HALT INSTRUCTION - print out all the registers and dump memory to .bin file
            printReg(cpu);
            break;
        default:
            cout << "Unknown Instruction" << endl;
            break;
    }
}

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