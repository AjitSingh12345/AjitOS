#include "stdint.h"
#include "stdio.h"

void _cdecl my_cstart(uint16_t driveNumber)
{
    // FAR is type of ptr that can access mem beyond current segment in segmented mem architectures
    const char far* my_far_string = "far string example";

    puts("Hello world from Ajit!\r\n");  // Changed output message
    printf("Formatted Example: %% %c %s %ls\r\n", 'a', "example string", my_far_string);
    printf("Various Formats: %d %i %x %p %o %hd %hi %hhu %hhd\r\n", 1234, -5678, 0xdead, 0xbeef, 012345, (short)27, (short)-42, (unsigned char)20, (signed char)-10);
    printf("Long Formats: %ld %lx %lld %llx\r\n", -100000000l, 0xdeadbeeful, 10200300400ll, 0xdeadbeeffeebdaedull);

    // Infinite loop to prevent exiting
    for (;;);
}