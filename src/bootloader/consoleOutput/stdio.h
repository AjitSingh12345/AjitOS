#ifndef MY_PRINTF_H
#define MY_PRINTF_H

#include "x86.h"

// Function prototypes for character and string output
void my_put_char(char character);           // Outputs a single character to the screen
void my_put_string(const char* string);     // Outputs a null-terminated string to the screen
void my_puts_far(const char far* string_far); // Outputs a far string (for segmented memory models) to the screen

// Definitions for printf state machine
#define MY_PRINTF_STATE_NORMAL         0   // Normal text processing
#define MY_PRINTF_STATE_LENGTH         1   // Processing length specifiers (h, l)
#define MY_PRINTF_STATE_LENGTH_SHORT   2   // Processing "hh" length specifier
#define MY_PRINTF_STATE_LENGTH_LONG    3   // Processing "ll" length specifier
#define MY_PRINTF_STATE_SPECIFIER      4   // Processing format specifiers (d, s, etc.)

// Definitions for length modifiers
#define MY_PRINTF_LENGTH_DEFAULT       0   // No length modifier
#define MY_PRINTF_LENGTH_SHORT_SHORT   1   // "hh" length modifier (char)
#define MY_PRINTF_LENGTH_SHORT         2   // "h" length modifier (short)
#define MY_PRINTF_LENGTH_LONG          3   // "l" length modifier (long)
#define MY_PRINTF_LENGTH_LONG_LONG     4   // "ll" length modifier (long long)

// Function prototype for custom printf implementation
void _cdecl my_printf(const char* format, ...); // Custom printf implementation

// Hexadecimal character set for number conversion
extern const char my_HexDigits[]; // Used for number to string conversion in hex format

// Function prototype for number formatting
int* my_printf_format_number(int* arg_ptr, int length, bool is_signed, int base); // Formats and prints numbers

#endif // MY_PRINTF_H
