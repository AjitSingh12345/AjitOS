[bits 32]

; void __attribute__((cdecl)) i686_GDT_Load(GDTDescriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);
global i686_GDT_Load
i686_GDT_Load:

    ; Save the previous base pointer and establish a new call frame
    push ebp             ; Save the previous call frame
    mov ebp, esp         ; Set up the new call frame

    ; Load the Global Descriptor Table (GDT)
    mov eax, [ebp + 8]   ; Load the pointer to the GDT descriptor into eax
    lgdt [eax]           ; Load the GDT using the lgdt instruction

    ; Reload the code segment
    mov eax, [ebp + 12]  ; Load the code segment selector into eax
    push eax             ; Push the selector onto the stack
    push _reload_cs      ; Push the address of the label to continue execution
    retf                 ; Far return to reload the code segment

_reload_cs:

    ; Reload the data segment registers
    mov ax, [ebp + 16]   ; Load the data segment selector into ax
    mov ds, ax           ; Reload DS register with the new segment selector
    mov es, ax           ; Reload ES register with the new segment selector
    mov fs, ax           ; Reload FS register with the new segment selector
    mov gs, ax           ; Reload GS register with the new segment selector
    mov ss, ax           ; Reload SS register with the new segment selector

    ; Restore the previous call frame and return
    mov esp, ebp         ; Restore the stack pointer to the previous frame
    pop ebp              ; Restore the previous base pointer
    ret                  ; Return to the caller
