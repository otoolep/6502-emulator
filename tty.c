#include <stdio.h>

void write_tty(unsigned char value)
{
    printf("%c", value);
    fflush(stdout);
    return;
}
