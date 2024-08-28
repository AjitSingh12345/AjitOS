#pragma once
#include <stdint.h>

// Enumeration of IDT flags, which define the properties of each entry in the IDT
typedef enum
{
    // Gate types
    IDT_FLAG_GATE_TASK              = 0x5,  // Task gate
    IDT_FLAG_GATE_16BIT_INT         = 0x6,  // 16-bit interrupt gate
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,  // 16-bit trap gate
    IDT_FLAG_GATE_32BIT_INT         = 0xE,  // 32-bit interrupt gate
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,  // 32-bit trap gate

    // Privilege levels
    IDT_FLAG_RING0                  = (0 << 5), // Ring 0 (highest privilege level)
    IDT_FLAG_RING1                  = (1 << 5), // Ring 1
    IDT_FLAG_RING2                  = (2 << 5), // Ring 2
    IDT_FLAG_RING3                  = (3 << 5), // Ring 3 (lowest privilege level)

    // Present flag
    IDT_FLAG_PRESENT                = 0x80, // Present flag (indicates if the gate is active)

} IDT_FLAGS;

// Function to initialize the Interrupt Descriptor Table (IDT)
// This function sets up the IDT entries and loads the IDT into the CPU's IDTR register
void i686_IDT_Initialize();

// Function to disable a gate in the IDT
// This function clears the "present" flag for the specified interrupt, disabling it
void i686_IDT_DisableGate(int interrupt);

// Function to enable a gate in the IDT
// This function sets the "present" flag for the specified interrupt, enabling it
void i686_IDT_EnableGate(int interrupt);

// Function to set an entry in the IDT
// This function configures the specified interrupt gate with the given handler base address, segment selector, and flags
void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);
