#include "memory.h"

// Function to copy a block of memory from source to destination
void* memcpy(void* destination, const void* source, uint16_t count)
{
    uint8_t* dest = (uint8_t *)destination;
    const uint8_t* src = (const uint8_t *)source;

    // Copy 'count' bytes from 'src' to 'dest'
    for (uint16_t i = 0; i < count; i++)
        dest[i] = src[i];

    return destination;
}

// Function to set a block of memory to a specific value
void* memset(void* memory, int value, uint16_t count)
{
    uint8_t* memPtr = (uint8_t *)memory;

    // Set 'count' bytes of 'memory' to 'value'
    for (uint16_t i = 0; i < count; i++)
        memPtr[i] = (uint8_t)value;

    return memory;
}

// Function to compare two blocks of memory
int memcmp(const void* block1, const void* block2, uint16_t count)
{
    const uint8_t* ptr1 = (const uint8_t *)block1;
    const uint8_t* ptr2 = (const uint8_t *)block2;

    // Compare 'count' bytes of 'block1' and 'block2'
    for (uint16_t i = 0; i < count; i++)
        if (ptr1[i] != ptr2[i])
            return 1;  // Return 1 if blocks differ

    return 0;  // Return 0 if blocks are identical
}
