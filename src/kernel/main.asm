org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A  ; Define newline characters (carriage return and line feed)

start:
	jmp main  ; Jump to the main routine


;
; Function to display a string on the screen
; Outputs characters until a null terminator is reached
; @param ds:si points to the string
;
puts:
	; Save registers that will be used
	push si
	push ax
	push bx 	

.print_loop
	lodsb		; Load byte from DS:SI into AL and increment SI
	or al, al	; Check if AL is zero (null terminator)
	jz .done    ; If null, jump to done

	mov ah, 0x0E	; BIOS teletype output function
	mov bh, 0		; Page number
	int 0x10		; BIOS interrupt to print character in AL

	jmp .print_loop	; Repeat the loop

.done:
	; Restore registers
	pop bx
	pop ax
	pop si
	ret

main: 
	; Initialize data segment registers
	mov ax, 0
	mov ds, ax
	mov es, ax

	; Initialize stack segment
	mov ss, ax
	mov sp, 0x7C00	; Set stack pointer

	; Display the "Hello world" message
	mov si, msg_hello
	call puts

	hlt  ; Halt the CPU

.halt:
	jmp .halt  ; Infinite loop to prevent further execution


msg_hello: db 'Hello world!', ENDL, 0  ; Define the message with a newline

times 510-($-$$) db 0  ; Fill the rest of the boot sector with zeros
dw 0AA55h  ; Boot sector signature
