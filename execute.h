#ifndef EXECUTE_H
#define EXECUTE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include "registers.h"

using namespace std;

//Structure takes in set of instructions in memory, identifies them, then executes in registers
struct Fetch{

    void execute(uint8_t *memory, Z80 *cpu, const int CYCLES){

        /*int CompletedCylces = 0;

        for (int i = 0; i < CYCLES; i++){
 
            decoder(memory, registers, i); //For jumps and multiple mem instructions will need to change from i

            if(memory[i] == 0x76){break;}
        }*/

        int i = 0; //'Instruction number' (where in mem)
        while(cpu->cycleCnt < CYCLES)
        {
            cpu->cycleCnt += decoder(memory, cpu, i); 
            if(memory[i++] == 0x76){break;} //Break on the halt instruction
        }

    }

    //Identification and execution of the instruction, returns the number of cycles run.
    int decoder(uint8_t *memory, Z80 *cpu, const int i){

        int cycles = 0;

        switch (memory[i]) {

            case 0x76: //HALT INSTRUCTION - print out all the registers & dump memory into .bin file
                cout << "HALT" << endl;
                printReg(cpu);
                memDump("memory.bin", memory);
                break;

            case 0x3e: //LOAD INSTRUCTION - 

                cout << "load n into A -> A:" 
                << memory[i]
                << endl;
                
                cycles += 7;
                break;

            case 0x02:

                cout << "Stores A into the memory location pointed to by BC." << endl;
                break;

            default:

                cout << "unknown" << endl;
                break;

        }

        return cycles;

    }


    //Function prints out registers of the z80
    void printReg(Z80 *cpu)
    {
        cout << "\n*=================Z80========================*" << endl;
        (cpu->regA) ? printf("regA: 0x%02X\n", *cpu->regA) : printf("regA: null\n");
        (cpu->Flags) ? printf("Flags: 0x%02X\n", *cpu->Flags) : printf("Flags: null\n");
        (cpu->regB) ? printf("regB: 0x%02X\n", *cpu->regB) : printf("regB: null\n");
        (cpu->regC) ? printf("regC: 0x%02X\n", *cpu->regC) : printf("regC: null\n");
        (cpu->regD) ? printf("regD: 0x%02X\n", *cpu->regD) : printf("regD: null\n");
        (cpu->regE) ? printf("regE: 0x%02X\n", *cpu->regE) : printf("regE: null\n");
        (cpu->regH) ? printf("regH: 0x%02X\n", *cpu->regH) : printf("regH: null\n");
        (cpu->regL) ? printf("regL: 0x%02X\n", *cpu->regL) : printf("regL: null\n");

        (cpu->reg_A) ? printf("reg_A: 0x%02X\n", *cpu->reg_A) : printf("reg_A: null\n");
        (cpu->reg_F) ? printf("reg_F: 0x%02X\n", *cpu->reg_F) : printf("reg_F: null\n");
        (cpu->reg_B) ? printf("reg_B: 0x%02X\n", *cpu->reg_B) : printf("reg_B: null\n");
        (cpu->reg_C) ? printf("reg_C: 0x%02X\n", *cpu->reg_C) : printf("reg_C: null\n");
        (cpu->reg_D) ? printf("reg_D: 0x%02X\n", *cpu->reg_D) : printf("reg_D: null\n");
        (cpu->reg_E) ? printf("reg_E: 0x%02X\n", *cpu->reg_E) : printf("reg_E: null\n");
        (cpu->reg_H) ? printf("reg_H: 0x%02X\n", *cpu->reg_H) : printf("reg_H: null\n");
        (cpu->reg_L) ? printf("reg_L: 0x%02X\n", *cpu->reg_L) : printf("reg_L: null\n");

        (cpu->reg_I) ? printf("reg_I: 0x%02X\n", *cpu->reg_I) : printf("reg_I: null\n");
        (cpu->reg_R) ? printf("reg_R: 0x%02X\n", *cpu->reg_R) : printf("reg_R: null\n");

        (cpu->reg_IX) ? printf("reg_IX: 0x%04X\n", *cpu->reg_IX) : printf("reg_IX: null\n");
        (cpu->reg_IY) ? printf("reg_IY: 0x%04X\n", *cpu->reg_IY) : printf("reg_IY: null\n");
        (cpu->reg_SP) ? printf("reg_SP: 0x%04X\n", *cpu->reg_SP) : printf("reg_SP: null\n");
        (cpu->reg_PC) ? printf("reg_PC: 0x%04X\n", *cpu->reg_PC) : printf("reg_PC: null\n");

        printf("Cycle Count: %d\n", cpu->cycleCnt);
        cout << "*============================================*\n" << endl;
    }

    //Function dumps memory into a .bin file  - Sebald's Code
    void memDump(const char *filename, uint8_t *memory) {
        FILE *fileptr;

        if(!(fileptr = fopen(filename, "wb"))) {
            fprintf(stderr, "z80_mem_dump: Cannot open destination file %s: %s\n", filename, strerror(errno));
            return;
        }

        if(fwrite(memory, 1, 65536, fileptr) != 65536) {
            cout << "z80_mem_dump: Couldn't write full memory dump" << endl;
            fclose(fileptr);
            return;
        }

        fclose(fileptr);
        cout << "Completed write to memory.bin :)" << endl;
    }

};

#endif