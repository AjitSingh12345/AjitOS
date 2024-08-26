bits 16

section _CODE_SEG class=CODE

;
; void _cdecl my_divide_u64_u32(uint64_t dividend, uint32_t divisor, uint64_t* quotientOut, uint32_t* remainderOut);
;
global _my_divide_u64_u32
_my_divide_u64_u32:

    ; Save the previous base pointer (bp) and establish a new call frame
    push bp             ; Save the old base pointer
    mov bp, sp          ; Set the new base pointer to the current stack pointer

    ; Save the necessary registers
    push bx

    ; Perform division of the upper 32 bits of the 64-bit dividend
    mov eax, [bp + 8]   ; Load upper 32 bits of the dividend into eax
    mov ecx, [bp + 12]  ; Load the divisor into ecx
    xor edx, edx        ; Clear edx to hold the upper bits of the dividend
    div ecx             ; Perform the division (eax:quotient, edx:remainder)

    ; Store the upper 32 bits of the quotient
    mov bx, [bp + 16]   ; Load the address of quotientOut into bx
    mov [bx + 4], eax   ; Store the upper 32 bits of the quotient

    ; Perform division of the lower 32 bits of the 64-bit dividend
    mov eax, [bp + 4]   ; Load lower 32 bits of the dividend into eax
                        ; edx already holds the remainder from the previous division
    div ecx             ; Perform the division (eax:quotient, edx:remainder)

    ; Store the results
    mov [bx], eax       ; Store the lower 32 bits of the quotient
    mov bx, [bp + 18]   ; Load the address of remainderOut into bx
    mov [bx], edx       ; Store the remainder

    ; Restore saved registers
    pop bx

    ; Restore the previous call frame
    mov sp, bp          ; Restore the stack pointer to the previous frame
    pop bp              ; Restore the old base pointer
    ret                 ; Return from the function

;
; void my_Video_WriteCharTeletype(char character, uint8_t page);
;
global _my_Video_WriteCharTeletype
_my_Video_WriteCharTeletype:

    ; Save the previous base pointer (bp) and establish a new call frame
    push bp             ; Save the old base pointer
    mov bp, sp          ; Set the new base pointer to the current stack pointer

    ; Save bx register
    push bx

    ; [bp + 0] - old base pointer
    ; [bp + 2] - return address (small memory model => 2 bytes)
    ; [bp + 4] - first argument (character)
    ; [bp + 6] - second argument (page)
    ; note: arguments are converted to words (stack can only push words)
    mov ah, 0Eh         ; BIOS teletype function
    mov al, [bp + 4]    ; Load the character to display into al
    mov bh, [bp + 6]    ; Load the page number into bh

    int 10h             ; BIOS interrupt to write the character

    ; Restore saved registers
    pop bx

    ; Restore the previous call frame
    mov sp, bp          ; Restore the stack pointer to the previous frame
    pop bp              ; Restore the old base pointer
    ret                 ; Return from the function
