#include "idt.h"
#include <stdint.h>
#include <util/binary.h>

// Structure representing an Interrupt Descriptor Table (IDT) entry
typedef struct
{
    uint16_t offset_low;                // Lower 16 bits of the handler function's address
    uint16_t selector;                  // Code segment selector in GDT or LDT
    uint8_t zero;                       // This byte is set to zero
    uint8_t type_attr;                  // Type and attributes of the IDT entry
    uint16_t offset_high;               // Upper 16 bits of the handler function's address
} __attribute__((packed)) IDTEntry;

// Structure representing the IDT descriptor
typedef struct
{
    uint16_t limit;                     // Size of the IDT minus one
    IDTEntry* base;                     // Pointer to the IDT array
} __attribute__((packed)) IDTDescriptor;

// Define the Interrupt Descriptor Table (IDT)
IDTEntry idt_table[256];

// Define the IDT descriptor
IDTDescriptor idt_descriptor = { sizeof(idt_table) - 1, idt_table };

// Function to load the IDT (assembly implementation)
void __attribute__((cdecl)) load_idt(IDTDescriptor* idt_descriptor);

// Function to set an entry in the IDT
void i686_IDT_SetGate(int interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags)
{
    idt_table[interrupt].offset_low = ((uint32_t)base) & 0xFFFF;
    idt_table[interrupt].selector = selector;
    idt_table[interrupt].zero = 0;
    idt_table[interrupt].type_attr = type_attr;
    idt_table[interrupt].offset_high = ((uint32_t)base >> 16) & 0xFFFF;
}

// Function to enable a gate in the IDT
void i686_IDT_EnableGate(int interrupt)
{
    FLAG_SET(idt_table[interrupt].type_attr, IDT_FLAG_PRESENT);
}

// Function to disable a gate in the IDT
void i686_IDT_DisableGate(int interrupt)
{
    FLAG_UNSET(idt_table[interrupt].type_attr, IDT_FLAG_PRESENT);
}

// Function to initialize the IDT
void i686_IDT_Initialize()
{
    i686_IDT_Load(&idt_descriptor);
}
