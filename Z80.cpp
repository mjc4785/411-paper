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
const string fileRun = filenameMAX;
/*
what I did 11/15/24
    - spaced out switch statement blocks
    - STARTED DIVIDE-8
    - jump and jump-relative.bin files both should work now. 
*/

/*
    Fixed addition flags. 
    I still dont know how the subtraction flags work 
    But I added in the bit 3 and 5 flags - they were undoccumented 
*/

//DEFINING IMPORTNANT THINGS=======================================================================================================
int const CYCLES = 1024;
int const MEMSIZE = 65536;
static uint8_t memory[MEMSIZE];
Z80 cpu;


//FUNCTION DEFINITIONS=========================================
int decode();
void printReg(Z80);
void setflags(uint8_t, bool, bool, bool, bool, bool);
uint8_t addFlags(uint8_t, uint8_t);
uint8_t subFlags(uint8_t, uint8_t);
uint8_t incFlags(uint8_t);
uint8_t twosComp(uint8_t);
void andFlags(uint8_t);


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
void z80_execute(){

    while(cpu.cycleCnt < CYCLES)
    {
        if(decode()) {break;} // if decode returns 1 (either halt or unknown inst), break
                              // otherwise, keep looping. 

        
    }

    // return CompletedCylces;
    return;
}

//Determine which instruction to run and execute it
int decode()
{
    uint8_t inst = z80_mem_read(cpu.reg_PC++);
    switch(inst)
    {




        //HALT------------------------------------------------------------------------
        case 0x76: //HALT INSTRUCTION - print out all the registers and dump memory to .bin file
            printReg(cpu);
            z80_mem_dump("memory.bin");
            return 1;
            break;

        //NOP INSTUCTION---------------------------------------------------------------
        case 0x00: //NOP INSTRUCTION
            cpu.cycleCnt += 4;
            break;
        
        //8-BIT GENERAL REGISTER LOADS------------------------------------------------
        case 0x3e: //LOAD INSTRUCTION - Load value at n into register A
            cpu.regA = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x06: //LOAD INSTRUCTION - Load value at n into register B
            cpu.regB = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x0e: //LOAD INSTRUCTION - Load value at n into register C
            cpu.regC = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x16: //LOAD INSTRUCTION - Load value at n into register D
            cpu.regD = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x1e: //LOAD INSTRUCTION - Load value at n into register E
            cpu.regE = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x26: //LOAD INSTRUCTION - Load value at n into register H
            cpu.regH = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;
        
        case 0x2e: //LOAD INSTRUCTION - Load value at n into register L
            cpu.regL = memory[int(cpu.reg_PC++)];
            cpu.cycleCnt += 7;
            break;

        //LOAD FROM REGISTER INSTRUCTIONS------------------------------------------
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
        
        case 0x87: //ADD INSTRUCTION - RegA += RegA
            cpu.regA = addFlags(cpu.regA, cpu.regA);
            cpu.cycleCnt += 4;
            break;
        
        case 0xc6: //ADD IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = addFlags(cpu.regA,memory[int(++cpu.reg_PC)]);
            cpu.cycleCnt += 7;
            break;
        





        //8-BIT SUBTRACTION ARITHMETIC-------------------------------------------------
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
        
        case 0xd6: //ADD IMMEDIATE INSTRUCTION - RegA += memory[pc+1] 
            cpu.regA = subFlags(cpu.regA,memory[int(++cpu.reg_PC)]);
            cpu.cycleCnt += 7;
            break;






        // JUMP INST ---------------------------------------------------------
        case 0xc3: //set pc to next value
            {
                //takes pc
                cpu.reg_PC = memory[int(cpu.reg_PC++)];
                // takes new ram address needed
                //increments it to that

                cpu.cycleCnt += 10; //cycle count for JP
                
                break;
            }

        case 0x18:
            cpu.reg_PC += memory[int(cpu.reg_PC++)];
            break;






        // INCREMENT INST ===================================================================================
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
        







//SHIFT IN STRUCNT ION S
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






        //UNIDENTIFIED INSTRUCTION--------------------------------------------------
        default:
            cout << "Unknown Instruction " << hex << int(inst) << endl;
            printReg(cpu);
            return 1;
            break;
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

//Edits the flag register according to the arithmetic opperation results
void setflags(uint8_t result, bool negative, bool halfCarry, bool overflow, bool subtraction, bool carry)
{
    //FLAGS: Sign | Zero | UNDOC:Copy of bit 5 of reesult| Half carry | UNDOC:Copy of bit 3 of reesult | Parity/oVerflow | add/subtract N | Carry
    cpu.Flags = 0; //Reset the flags for each result

    if (negative)     {cpu.Flags |= 0b10000000;}  //(bit 7) S-Flag  [0:(+/0)result | 1:(-)result]
    if (result == 0)    {cpu.Flags |= 0b01000000;}  //(bit 6) Z-flag  [0: result!=0 | 1: result==0]
    cpu.Flags |= ((result >> 4) & 1) << 5; //(bit 5) Undoccumented - 5th bit of the result
    if (halfCarry)      {cpu.Flags |= 0b00010000;}  //(bit 4) H-Flag [0:carry absent | 1: carry present]
    cpu.Flags |= ((result >> 2) & 1) << 3; //(bit 3) Undoccumented - 3rd bit of the result
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
    uint8_t negativeReg2 = twosComp(reg2);
    uint8_t diff = reg1 + negativeReg2;

    cout << bitset<8>(reg1) << "-" << bitset<8>(reg2) << "=" << bitset<8>(diff)  << "=" << int(diff) << endl;

    bool halfCarry = (((reg1 & 0xf) + (negativeReg2 & 0xf)) & 0x10) == 0x10; //FROM ROBM.DEV
    bool overflow = ((reg1 ^ diff) & (negativeReg2 ^ diff) & 0x80) != 0; //If the carries between bits dont result to 0, there was overflow
    bool carry = reg2 > reg1; //If reg 2 is bigger, then there was a carry
    setflags(int(diff),(reg2 > reg1), halfCarry, overflow, true, carry);
    return diff;
}

//Does the logic for incramenting a register by 1 - Cannot use addFlags, bc carry bit is unaffected
uint8_t incFlags(uint8_t reg1)
{
    int32_t sum = reg1 + uint8_t(1);
    bool halfCarry = (sum ^ reg1 ^ uint8_t(1)) & 0x10; //Demonstrated in class
    bool overflow = (reg1 == 0x7F); //Overflows at 0x80
    bool carry = cpu.Flags & 1; //CARRY IS UNAFFECTED
    setflags(sum, (sum < 0), halfCarry, overflow, false, carry);
    return sum;
}

uint8_t twosComp(uint8_t reg)
    {return (~reg) + 1;}

//MAIN==============================================================================================================================
int main(){
    cout << "Max Castle is feeling splendid" << endl; //File running check

    //z80_mem_load(fileRun.c_str()); //Load into memory
    z80_mem_write(0x00, 0x06);//load B
    z80_mem_write(0x01, 0x7F);//goes into b 
    z80_mem_write(0x02, 0x3e);//load a
    z80_mem_write(0x03, 0x64);//goes into b 
    //z80_mem_write(0x04, 0x80); //a= a+b
    z80_mem_write(0x04, 0x47);
    z80_mem_write(0x05, 0x76);//halt
    
    for (int i =0; i < MEMSIZE; i++)
    {
        if(memory[i] != 0)
            {printf("ram[%04x] = %02x\n", i, memory[i]);}
    }

    z80_execute();

    return 0;
}