#include "gdt.h"
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

// Enumeration of GDT access flags
typedef enum
{
    GDT_ACCESS_CODE_READABLE                = 0x02,
    GDT_ACCESS_DATA_WRITABLE                = 0x02,

    GDT_ACCESS_CODE_CONFORMING              = 0x04,
    GDT_ACCESS_DATA_DIRECTION_UP            = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN          = 0x04,

    GDT_ACCESS_DATA_SEGMENT                 = 0x10,
    GDT_ACCESS_CODE_SEGMENT                 = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS               = 0x00,

    GDT_ACCESS_RING0                        = 0x00,
    GDT_ACCESS_RING1                        = 0x20,
    GDT_ACCESS_RING2                        = 0x40,
    GDT_ACCESS_RING3                        = 0x60,

    GDT_ACCESS_PRESENT                      = 0x80,

} GDT_ACCESS;

// Enumeration of GDT flags
typedef enum 
{
    GDT_FLAG_64BIT                          = 0x20,
    GDT_FLAG_32BIT                          = 0x40,
    GDT_FLAG_16BIT                          = 0x00,

    GDT_FLAG_GRANULARITY_BYTE               = 0x00,
    GDT_FLAG_GRANULARITY_PAGE               = 0x80,
} GDT_FLAGS;

// Helper macros for creating GDT entries
#define GDT_LIMIT_LOW(limit)                ((limit) & 0xFFFF)
#define GDT_BASE_LOW(base)                  ((base) & 0xFFFF)
#define GDT_BASE_MIDDLE(base)               (((base) >> 16) & 0xFF)
#define GDT_FLAGS_LIMIT_HI(limit, flags)    (((((limit) >> 16) & 0xF) | ((flags) & 0xF0)))
#define GDT_BASE_HIGH(base)                 (((base) >> 24) & 0xFF)

// Macro for defining a GDT entry
#define DEFINE_GDT_ENTRY(base, limit, access, flags) {             \
    GDT_LIMIT_LOW(limit),                                           \
    GDT_BASE_LOW(base),                                             \
    GDT_BASE_MIDDLE(base),                                          \
    access,                                                         \
    GDT_FLAGS_LIMIT_HI(limit, flags),                               \
    GDT_BASE_HIGH(base)                                             \
}

// Global Descriptor Table (GDT)
GDTEntry gdt_table[] = {
    // Null descriptor
    DEFINE_GDT_ENTRY(0, 0, 0, 0),

    // Kernel 32-bit code segment
    DEFINE_GDT_ENTRY(0,
                     0xFFFFF,
                     GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
                     GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_PAGE),

    // Kernel 32-bit data segment
    DEFINE_GDT_ENTRY(0,
                     0xFFFFF,
                     GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE,
                     GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_PAGE),
};

// GDT Descriptor
GDTDescriptor gdt_descriptor = { sizeof(gdt_table) - 1, gdt_table };

// Function to load the GDT
void __attribute__((cdecl)) load_gdt(GDTDescriptor* descriptor, uint16_t code_segment, uint16_t data_segment);

// Function to initialize the GDT
void initialize_gdt()
{
    load_gdt(&gdt_descriptor, code_segment, data_segment);
}
