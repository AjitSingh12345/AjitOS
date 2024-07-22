org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A 	; so we dont have to redefine every time to print '\n'

start:
	jmp main


;
; Function to print string to the screen
; Prints chars till encounters a null char
; @param ds:si pts to a string
;
puts:
	; save regs we will modify
	push si
	push ax
	push bx 	

.loop
	lodsb		; this instruc loads next byte/word from DS:SI into AL/AX/AEX, then incs SI by # bytes loaded
	or al, al	; verify if next char is null
	jz .done

	mov ah, 0x0E	; call BIOS interrupt
	mov bh, 0		; set page # to 0
	int 0x10		

	jmp .loop		; keep printing chars

.done
	; restore regs to prev state
	pop bx
	pop ax
	pop si
	ret

main: 
	; setup data segments
	mov ax, 0
	mov ds, ax
	mov es, ax

	; setup stack
	mov ss, ax
	mov sp, 0x7C00	; stack growth downwards from where vm are loaded in memory 

	; print hello world msg
	mov si, msg_hello
	call puts

	hlt

.halt:
	jmp .halt


msg_hello: db 'Hello world!', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h