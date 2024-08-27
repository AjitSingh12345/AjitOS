#pragma once
#include <stdint.h>

// Function to copy a block of memory from source to destination
void* memcpy(void* destination, const void* source, uint16_t count);

// Function to set a block of memory to a specific value
void* memset(void* memory, int value, uint16_t count);

// Function to compare two blocks of memory
int memcmp(const void* block1, const void* block2, uint16_t count);
