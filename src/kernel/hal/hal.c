#include "hal.h"
#include <arch/i686/gdt.h>
#include <arch/i686/idt.h>

void HAL_Init()
{
    i686_GDT_Init();
    i686_IDT_Init();
}