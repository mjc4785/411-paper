#ifndef EXECUTE_H
#define EXECUTE_H

#include <iostream>
#include <string>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include "registers.h"

using namespace std;

struct Fetch{

    //add in Z80 registers 
    // make memory an array

    void execute(uint8_t memory, const int CYCLES){

        int CompletedCylces = 0;

        for (int i = 0; i < CYCLES; i++){

            decoder(memory, i);

            if(memory[i] == 0x76){break;}

        }

    }

    int decoder(uint8_t memory, const int i){

        int cycles = 0;

        switch (memory[]) {

            case 0x76:

                cout << "HALT" << endl;
                break;

            case 0x3e:

                cout << "load n into A -> A:" 
                << memory[]
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

};

#endif