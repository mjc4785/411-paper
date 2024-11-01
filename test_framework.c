#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define NUM_CYCLES 1024

extern void z80_init(void);
extern int z80_execute(int cycles);
static uint8_t ram[65536];

void z80_mem_write(uint16_t addr, uint8_t value) {
    ram[addr] = value;
}

uint8_t z80_mem_read(uint16_t addr) {
    return ram[addr];
}

void z80_mem_write16(uint16_t addr, uint16_t value) {
    ram[addr] = (uint8_t)(value & 0xff);
    ram[addr + 1] = (uint8_t)(value >> 8);
}

uint16_t z80_mem_read16(uint16_t addr) {
    return ((uint16_t)ram[addr]) | (((uint16_t)ram[addr + 1]) << 8);
}

void z80_mem_dump(const char *fn) {
    FILE *fp;

    if(!(fp = fopen(fn, "wb"))) {
        fprintf(stderr, "z80_mem_dump: Cannot open destination file %s: %s\n",
                fn, strerror(errno));
        return;
    }

    if(fwrite(ram, 1, 65536, fp) != 65536) {
        fprintf(stderr, "z80_mem_dump: Couldn't write full memory dump\n");
        fclose(fp);
        return;
    }

    fclose(fp);
}

void z80_mem_load(const char *fn) {
    FILE *fp;

    if(!(fp = fopen(fn, "rb"))) {
        fprintf(stderr, "z80_mem_load: Cannot open source file %s: %s\n",
                fn, strerror(errno));
        return;
    }

    if(!fread(ram, 1, 65536, fp)) {
        fprintf(stderr, "z80_mem_read: Couldn't read program binary\n");
        fclose(fp);
        return;
    }

    fclose(fp);
}
       
int main(int argc, char *argv[]) {
    int ncycles;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.bin\n", argv[0]);
        return -1;
    }

    z80_init();
    z80_mem_load(argv[1]);
    ncycles = z80_execute(NUM_CYCLES);

    printf("Ran %d cycles\n", ncycles);
    return 0;
}