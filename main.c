#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "M6502.h"
#include "memory_map.h"
#include "tty.h"

#ifndef PROG_NAME
#define PROG_NAME "6502"
#endif

/*
 * 6502 CPU state and memory space.
 */
M6502 cpu;
byte ram[TOP_OF_RAM];

int usage(const char *error)
{
    int rc = EXIT_SUCCESS;

    if (error) {
        printf("Error: %s\n", error);
        rc = EXIT_FAILURE;
    }

    printf("Usage: %s [options] file\n", PROG_NAME);
    printf(" file: File of 6502 machine code.\n");
    printf(" -l: Load address. This value will also be loaded into the reset vector.\n");
    printf("                   Defaults to 0x0.\n");
    printf(" -h: Display the help.\n");

    exit(rc);
}

void set_reset_vector(word value)
{
    pair addr = (pair) value;
    ram[RESET_LSB] = addr.B.l;
    ram[RESET_MSB] = addr.B.h;
}

void load_machine_code(const char* file, word load_addr)
{
    FILE *code;
    int val;

    code = fopen(file, "r");
    if (!code) {
        usage("Unable to open file.");
    }

    while ((val = fgetc(code)) != EOF) {
        ram[load_addr++] = val;
        if (load_addr > TOP_OF_RAM) {
            usage("Machine code file too large.");
        }
    }

    fclose(code);
}

int main(int argc, char *argv[])
{
    int opt = 0;
    word load_addr = 0;

    while ((opt = getopt(argc, argv, "hl:")) != -1) {
        switch (opt) {
            case 'l':
                load_addr = atoi(optarg);
                if (load_addr > TOP_OF_RAM) {
                    usage("Illegal load address.");
                }
                break;
            case 'h':
                usage(NULL);
                break;
            default:
                usage("Unrecognised option.");
                break;
        }
    }

    if (optind >= argc) {
        usage("No machine code file supplied.");
    }

    /*
     * Power-up and reset!
     */

    printf("Loading code at 0x%x.\n", load_addr);
    set_reset_vector(load_addr);
    load_machine_code(argv[optind], load_addr);

    printf("Resetting the CPU.\n");
    Reset6502(&cpu);

    printf("Starting the CPU.\n");
    Run6502(&cpu);

    return 0;
}

/*
 * M6502 interface functions.
 */

void Wr6502(register word Addr, register byte Value)
{
    /* Is A15 high? */
    if ((Addr & A15_MASK) == A15_MASK) {
        write_tty((char) Value);
    } else {
        ram[Addr] = Value;
    }
}

byte Rd6502(register word Addr)
{
    /* Is A15 high? */
    if ((Addr & A15_MASK) == A15_MASK) {
        return 0; /* Output Device is write-only. */
    } else {
        return ram[Addr];
    }
}

byte Loop6502(register M6502 *R)
{
    return 0;
}

byte Patch6502(register byte Op, register M6502 *R)
{
    return 0;
}
