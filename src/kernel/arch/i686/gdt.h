#ifndef GDT_OPERATIONS_H
#define GDT_OPERATIONS_H

#include <stdint.h>

// Structure representing a Global Descriptor Table (GDT) entry
typedef struct
{
    uint16_t limit_low;                  // Limit (bits 0-15)
    uint16_t base_low;                   // Base (bits 0-15)
    uint8_t base_middle;                 // Base (bits 16-23)
    uint8_t access;                      // Access byte
    uint8_t flags_limit_hi;              // Limit (bits 16-19) | Flags
    uint8_t base_high;                   // Base (bits 24-31)
} __attribute__((packed)) GDTEntry;

// Structure representing the GDT descriptor
typedef struct
{
    uint16_t limit;                      // Size of the GDT minus one
    GDTEntry* base;                      // Pointer to the GDT
} __attribute__((packed)) GDTDescriptor;

// External declarations for the GDT table and descriptor
extern GDTEntry gdt_table[];
extern GDTDescriptor gdt_descriptor;

// Function to load the GDT
void __attribute__((cdecl)) load_gdt(GDTDescriptor* descriptor, uint16_t code_segment, uint16_t data_segment);

// Function to initialize the GDT
void initialize_gdt();

#endif // GDT_OPERATIONS_H
