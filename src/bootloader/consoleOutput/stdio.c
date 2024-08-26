#include "stdio.h"
#include "x86.h"

// Function to output a single character
void my_put_char(char character)
{
    x86_Video_WriteCharTeletype(character, 0);
}

// Function to output a null-terminated string
void my_put_string(const char* string)
{
    while (*string)
    {
        my_put_char(*string);
        string++;
    }
}

// Function to output a far string (for segmented memory models)
void my_puts_far(const char far* string_far)
{
    while (*string_far)
    {
        my_put_char(*string_far);
        string_far++;
    }
}

// Definitions for printf state machine
#define MY_PRINTF_STATE_NORMAL         0
#define MY_PRINTF_STATE_LENGTH         1
#define MY_PRINTF_STATE_LENGTH_SHORT   2
#define MY_PRINTF_STATE_LENGTH_LONG    3
#define MY_PRINTF_STATE_SPECIFIER      4

// Definitions for length modifiers
#define MY_PRINTF_LENGTH_DEFAULT       0
#define MY_PRINTF_LENGTH_SHORT_SHORT   1
#define MY_PRINTF_LENGTH_SHORT         2
#define MY_PRINTF_LENGTH_LONG          3
#define MY_PRINTF_LENGTH_LONG_LONG     4

// Forward declaration of number formatting function
int* my_printf_format_number(int* arg_ptr, int length, bool is_signed, int base);

// Custom printf implementation
void _cdecl my_printf(const char* format, ...)
{
    int* args = (int*)&format;
    int current_state = MY_PRINTF_STATE_NORMAL;
    int length_modifier = MY_PRINTF_LENGTH_DEFAULT;
    int base = 10;
    bool is_signed = false;

    args++; // Move to the first argument after format

    while (*format)
    {
        switch (current_state)
        {
            case MY_PRINTF_STATE_NORMAL:
                if (*format == '%')
                {
                    current_state = MY_PRINTF_STATE_LENGTH;
                }
                else
                {
                    my_put_char(*format);
                }
                break;

            case MY_PRINTF_STATE_LENGTH:
                if (*format == 'h')
                {
                    length_modifier = MY_PRINTF_LENGTH_SHORT;
                    current_state = MY_PRINTF_STATE_LENGTH_SHORT;
                }
                else if (*format == 'l')
                {
                    length_modifier = MY_PRINTF_LENGTH_LONG;
                    current_state = MY_PRINTF_STATE_LENGTH_LONG;
                }
                else
                {
                    goto PROCESS_SPECIFIER;
                }
                break;

            case MY_PRINTF_STATE_LENGTH_SHORT:
                if (*format == 'h')
                {
                    length_modifier = MY_PRINTF_LENGTH_SHORT_SHORT;
                    current_state = MY_PRINTF_STATE_SPECIFIER;
                }
                else
                {
                    goto PROCESS_SPECIFIER;
                }
                break;

            case MY_PRINTF_STATE_LENGTH_LONG:
                if (*format == 'l')
                {
                    length_modifier = MY_PRINTF_LENGTH_LONG_LONG;
                    current_state = MY_PRINTF_STATE_SPECIFIER;
                }
                else
                {
                    goto PROCESS_SPECIFIER;
                }
                break;

            case MY_PRINTF_STATE_SPECIFIER:
            PROCESS_SPECIFIER:
                switch (*format)
                {
                    case 'c':
                        my_put_char((char)*args);
                        args++;
                        break;

                    case 's':
                        if (length_modifier == MY_PRINTF_LENGTH_LONG || length_modifier == MY_PRINTF_LENGTH_LONG_LONG)
                        {
                            my_puts_far(*(const char far**)args);
                            args += 2;
                        }
                        else
                        {
                            my_put_string(*(const char**)args);
                            args++;
                        }
                        break;

                    case '%':
                        my_put_char('%');
                        break;

                    case 'd':
                    case 'i':
                        base = 10;
                        is_signed = true;
                        args = my_printf_format_number(args, length_modifier, is_signed, base);
                        break;

                    case 'u':
                        base = 10;
                        is_signed = false;
                        args = my_printf_format_number(args, length_modifier, is_signed, base);
                        break;

                    case 'x':
                    case 'X':
                    case 'p':
                        base = 16;
                        is_signed = false;
                        args = my_printf_format_number(args, length_modifier, is_signed, base);
                        break;

                    case 'o':
                        base = 8;
                        is_signed = false;
                        args = my_printf_format_number(args, length_modifier, is_signed, base);
                        break;

                    default:
                        // Unknown specifier, ignore
                        break;
                }

                // Reset state after processing specifier
                current_state = MY_PRINTF_STATE_NORMAL;
                length_modifier = MY_PRINTF_LENGTH_DEFAULT;
                base = 10;
                is_signed = false;
                break;
        }

        format++;
    }
}

// Hexadecimal character set for number conversion
const char my_HexDigits[] = "0123456789abcdef";

// Function to format and print numbers based on given parameters
int* my_printf_format_number(int* arg_ptr, int length, bool is_signed, int base)
{
    char temp_buffer[32];
    unsigned long long number;
    int sign_flag = 1;
    int buffer_index = 0;

    // Handle different length modifiers
    switch (length)
    {
        case MY_PRINTF_LENGTH_SHORT_SHORT:
        case MY_PRINTF_LENGTH_SHORT:
        case MY_PRINTF_LENGTH_DEFAULT:
            if (is_signed)
            {
                int temp = *arg_ptr;
                if (temp < 0)
                {
                    temp = -temp;
                    sign_flag = -1;
                }
                number = (unsigned long long)temp;
            }
            else
            {
                number = *(unsigned int*)arg_ptr;
            }
            arg_ptr++;
            break;

        case MY_PRINTF_LENGTH_LONG:
            if (is_signed)
            {
                long temp = *(long*)arg_ptr;
                if (temp < 0)
                {
                    temp = -temp;
                    sign_flag = -1;
                }
                number = (unsigned long long)temp;
            }
            else
            {
                number = *(unsigned long*)arg_ptr;
            }
            arg_ptr += 2;
            break;

        case MY_PRINTF_LENGTH_LONG_LONG:
            if (is_signed)
            {
                long long temp = *(long long*)arg_ptr;
                if (temp < 0)
                {
                    temp = -temp;
                    sign_flag = -1;
                }
                number = (unsigned long long)temp;
            }
            else
            {
                number = *(unsigned long long*)arg_ptr;
            }
            arg_ptr += 4;
            break;
    }

    // Convert number to string in the specified base
    do
    {
        uint32_t remainder;
        x86_div64_32(number, base, &number, &remainder);
        temp_buffer[buffer_index++] = my_HexDigits[remainder];
    } while (number > 0);

    // Add negative sign if necessary
    if (is_signed && sign_flag < 0)
    {
        temp_buffer[buffer_index++] = '-';
    }

    // Output the number in reverse order
    while (--buffer_index >= 0)
    {
        my_put_char(temp_buffer[buffer_index]);
    }

    return arg_ptr;
}
