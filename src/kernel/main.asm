org 0x0
bits 16

%define ENDL 0x0D, 0x0A  ; Define newline characters (carriage return and line feed)
%define BACKSPACE 0x08

start:
    ; print hello world message
    mov si, msg_hello
    call puts

command_loop:
    ; print prompt
    mov si, prompt
    call puts

    ; read command
    call read_command

    ; process command
    mov si, buffer
    call process_command

    jmp command_loop

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
    lodsb      ; Load byte from DS:SI into AL and increment SI
    or al, al  ; Check if AL is zero (null terminator)
    jz .done   ; If null, jump to done

    mov ah, 0x0E    ; BIOS teletype output function
    mov bh, 0       ; Page number
    int 0x10        ; BIOS interrupt to print character in AL

    jmp .print_loop ; Repeat the loop

.done:
    ; Restore registers
    pop bx
    pop ax
    pop si
    ret

;
; Function to read a command from the user
; @param es:di points to the buffer to store the command
;
read_command:
    mov di, buffer
    mov cx, 128  ; Limit the command length to 128 characters

.read_loop:
    mov ah, 0x00  ; BIOS function to read a character
    int 0x16      ; Read character into AL
    cmp al, 0x0D  ; Check if Enter key (carriage return) is pressed
    je .end_read
    cmp al, BACKSPACE  ; Check if Backspace is pressed
    je .handle_backspace

    ; Echo character to the screen
    mov ah, 0x0E  ; BIOS teletype output function
    mov bh, 0     ; Page number
    int 0x10      ; BIOS interrupt to print character in AL

    stosb         ; Store AL into ES:DI and increment DI
    loop .read_loop

.handle_backspace:
    cmp di, buffer  ; Check if we are at the start of the buffer
    je .read_loop   ; If at the start, do nothing

    dec di          ; Move back one position
    mov al, BACKSPACE
    mov ah, 0x0E
    int 0x10        ; BIOS interrupt to handle backspace
    mov al, ' '
    int 0x10        ; Print a space to overwrite the character
    mov al, BACKSPACE
    int 0x10        ; Move cursor back again
    jmp .read_loop

.end_read:
    mov byte [di], 0  ; Null terminate the string
    ret

;
; Function to process a command
; @param ds:si points to the command
;
process_command:
    ; Print newline before the command output
    mov si, newline
    call puts

    ; Check if the command is empty
    mov si, buffer
    cmp byte [si], 0
    je .done_processing

    ; Check for "echo " command
    mov di, cmd_echo
    mov cx, 5  ; Length of "echo "
    repe cmpsb
    jne .unknown_command
    
    ; Echo command found, print the rest of the string
    call puts
    ; Print newline after the command output
    mov si, newline
    call puts
    jmp .done_processing

.unknown_command:
    ; Unknown command, print error message
    mov si, msg_unknown_command
    call puts
    ; Print newline after the error message
    mov si, newline
    call puts

.done_processing:
    ret

newline: db ENDL, 0
msg_hello: db 'Hello world from AjitOS Kernel!', ENDL, 0  ; Define the message with a newline
prompt: db 'AjitOS> ', 0
msg_unknown_command: db 'Unknown command', ENDL, 0
cmd_echo: db 'echo ', 0  ; Note the space after "echo"

buffer: times 128 db 0
