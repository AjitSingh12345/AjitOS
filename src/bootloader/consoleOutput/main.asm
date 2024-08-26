bits 16

section _START class=CODE

extern _my_cstart_
global _entry_point

_entry_point:
    cli
    ; Initialize stack segment and pointer
    mov ax, ds
    mov ss, ax
    xor sp, sp
    mov bp, sp
    sti

    ; Boot drive expected in DL, passing it to _my_cstart_
    xor dh, dh
    push dx
    call _my_cstart_

    cli
    hlt
