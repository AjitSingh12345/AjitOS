org 0x0
bits 16

%define ENDL 0x0D, 0x0A  ; Define newline characters (carriage return and line feed)

start:
	; print hello world message
    mov si, msg_hello
    call puts

.halt:
    ; Infinite loop to prevent further execution
	cli
    hlt

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

msg_hello: db 'Hello world from AjitOS Kernel!', ENDL, 0  ; Define the message with a newline
