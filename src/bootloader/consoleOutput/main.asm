bits 16

section _START class=CODE

extern _cstart_
global _entry_point

_entry_point:
    cli
    ; Initialize stack segment and pointer
    mov ax, ds
    mov ss, ax
    xor sp, sp
    mov bp, sp
    sti

    ; Boot drive expected in DL, passing it to _cstart_
    xor dh, dh
    push dx
    call _cstart_

    cli
    hlt
