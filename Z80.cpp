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
uint8_t addFlags(uint8_t, uint8_t);
uint8_t subFlags(uint8_t, uint8_t);


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
        
        case 0xc6: //ADD IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = addFlags(cpu.regA,memory[int(++cpu.reg_PC)]);
            cpu.cycleCnt += 7;
            break;
        
        //8-BIT SUBTRACTION ARITHMETIC-------------------------------------------------
        /*case 0x90: //SUBTRACTION INSTRUCTiON - RegA -= RegB
            cpu.regA = subFlags(cpu.regA, cpu.regB);
            cpu.cycleCnt += 4;
            break;*/

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


//Edits the flag register according to the arithmetic opperation results - I DONT KNOW IF THIS WORKS YET
void setflags(uint8_t result, bool halfCarry, bool overflow, bool subtraction, bool carry)
{
    //FLAGS: Sign | Zero | unused X | Half carry | unused X | Parity/oVerflow | add/subtract N | Carry
    cpu.Flags = 0; //Reset the flags for each result

    if (result < 0)     {cpu.Flags |= 0b10000000;}  //(bit 7) S-Flag  [0:(+/0)result | 1:(-)result]
    if (result == 0)    {cpu.Flags |= 0b01000000;}  //(bit 6) Z-flag  [0: result!=0 | 1: result==0]
    //Bit 5 unused 
    if (halfCarry)      {cpu.Flags |= 0b00010000;}  //(bit 4) H-Flag [0:carry absent | 1: carry present]
    //Bit 3 unused
    if(overflow)        {cpu.Flags |= 0b00000100;}  //(bit 2) P/V-Flag [0: no overflow or odd number of 1 bits]
    if(subtraction)     {cpu.Flags |= 0b00000010;}  //(bit 1) N-Flag [0:Addition | 1: Subtraction]
    if(carry)           {cpu.Flags |= 0b00000001;}  //(bit 0) C-Flag [0: no carry/borrow | 1: carry/borrow]
}

//Does the logic for adding a register to another, returns their sum and sets their flags.
uint8_t addFlags(uint8_t reg1, uint8_t reg2)
{
    uint8_t sum = reg1 + reg2;
    bool halfCarry = (((reg1 & 0xf) + (reg2 & 0xf)) & 0x10) == 0x10; //FROM ROBM.DEV
    bool overflow = ((reg1 ^ sum) & (reg2 ^ sum) & 0x80) != 0; //If the carries between bits dont result to 0, there was overflow
    bool carry = (reg1 + reg2) > (pow(2,8)-1); //If sum is more than 8 bits can hold, there was a carry
    setflags(sum,halfCarry, overflow, false, carry);
    return sum;
}

//Does the logic for subtracting a register to another, returns their differents and sets their flags. 
uint8_t subFlags(uint8_t reg1, uint8_t reg2)
{
    //uint8_t diff = reg1 - reg2;
    return 0;
}

//MAIN==============================================================================================================================
int main(){
    cout << "Max Castle is feeling up-really-cool-guy" << endl; //File running check

    //z80_mem_load(fileRun.c_str()); //Load into memory
    z80_mem_write(0x00, 0x06);//load R
    z80_mem_write(0x01, 0x01);//into R
    z80_mem_write(0x02, 0x3e);//load a
    z80_mem_write(0x03, 0x02);//into a
    //z80_mem_write(0x04, 0x90);//a = a-b 
    z80_mem_write(0x05, 0x76);//halt
    
    for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }

    z80_execute();

    return 0;
}