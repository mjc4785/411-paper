[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8sT2TSr2)

Names: Emma Chaney and Maxwell Castle 
Date: 12/10/24
Teacher: Lawrence Sebald 

Z80 emulator Project ----------------------------------------------------------------------------------------------------------

In this project, we designed and implemented a CPU emulator for a subset of the 8-bit Z80 CPU. Our code accurately emulated the required opcodes, including any bugs and undocumented effects (as outlined in the resources provided). Specifically, we implemented a type of emulation called interpretation. In interpretation, we read an opcode, executed a segment of code that implemented the functionality of that opcode, and repeated the process until we completed the 
required number of cycles. We were responsible for maintaining the state of the CPU, including all its registers, throughout this process.

----------------------------------------------------------------------------------------------------------

-GLOBAL ----------------------------------------------------------------------------------------------------------
    - in the global section of our code we load in some libraries and make some global variables as well as create some strings for easy read in of the bin files 
         
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
          const string filenameMAX = "C:\\411\\bit.bin"; //MAX, PUT .BIN AFTER THE GODDAMN OATH NAME
          const string fileRun = filenameMAX;
          
          
          //DEFINING IMPORTNANT THINGS=======================================================================================================
          int const CYCLES = 1024;
          int const MEMSIZE = 65536;
          bool INTERUPT_MODE1 = true;
          bool INTERUPT_MODE2 = true;
          static uint8_t memory[MEMSIZE];
          Z80 cpu;

- MAIN ----------------------------------------------------------------------------------------------------------

  -in the main section of our code the first thing that is done is a read in of the .bin files used to test our code. the being one of the following
        i)-         Basic example (just does a halt)  
        ii)-        Load the basic (a, b, c, d, e, h, and l) registers  
        iii)-       Do an unconditional absolute jump and some loads afterwards  
        iv)-        Do an unconditional relative jump and some loads afterwards  
        v)-         Do a subroutine call and some loads in it  
        vi)-        40 + 2 = 42  
        vii)-       40 - 2 = 38  
        viii)-      10 * 4 + 2 = 42  
        ix)-         -80 / 8 + 2 = -8  
        x)-         IX/IY with loads and adds  
        xi)-        Shift instructions  
        xii)-       Sixteen bit arithmetic  
        xiii)-      Rotate instructions  
        xiv)-       Testing BIT instructions with IX  
        xv)-        Same as the above, but not with IX

  once the bin file is read in which is split up amongst the ram memory in the code memory[]
  
- DECODE ----------------------------------------------------------------------------------------------------------
    - the decode function takes the current PC value and reads the opcode into a massive switch statment. the switch statment will evaluate the value in PC and run code accordingly in one of the following below tables...
  
- MAIN-TABLE ----------------------------------------------------------------------------------------------------------
    - the main table is assumed when an opcode is read. this main table consists of 256 operations or transitions. 
  
- ED ----------------------------------------------------------------------------------------------------------
    - this handles instructions with the ED extention infornt of them. Many of the cases are input output, and are thus not implemented in the scope of this project. But are still handled as NOPs.
  
- CB ----------------------------------------------------------------------------------------------------------
    - this table does shift and bit operations. when a shift or bit operation is read in it is being the cb opcode
  
- DD ----------------------------------------------------------------------------------------------------------
    - this is the prefix for IX operations. after reading in dd: ed, cb, or another opcode could follow 
  
- FD ----------------------------------------------------------------------------------------------------------
    - this is the prefix for IY operations. after reading in fd: ed, cb, or another opcode could follow 

- SOURCES ----------------------------------------------------------------------------------------------------------
    - **http://www.emulation.org/EMUL8/HOWTO.html** — A document going over the very basics of writing an emulator.
    - **http://www.zilog.com/docs/z80/z80cpu_um.pdf** — The official low-level manual for the Z80 CPU
    - **http://z80.info/zip/z80-documented.pdf** — Documentation of undocumented opcodes/effects and errata from the official documentation
    - **http://z80.info/** — A site full of resources on the Z80 CPU.
    - **https://github.com/vhelin/wla-dx** — The assembler used to make the example programs below
    - **https://www.robm.dev/articles/gameboy-half-carry**/ -- (technically about the Game Boy's CPU, but the idea is the same since both the Game Boy's CPU and the Z80 are derived from the 8080).
    - **https://www.zilog.com/docs/z80/um0080.pdf** -- a website to help us with the entire emulator. 
    - **https://jgmalcolm.com/z80/beginner/flag#CarryFlag** -- a website to help us understand the flags 
    - **https://clrhome.org/table/#add** -- website with all of the op codes and other tables 
