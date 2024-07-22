org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A  ; Define newline characters for easy printing

;
; Boot sector setup and FAT12 header
;
jmp short start
nop

; Boot Sector Parameters
bdb_oem:                    db 'MSWIN4.1'           ; OEM Identifier
bdb_bytes_per_sector:       dw 512                  ; Bytes per sector
bdb_sectors_per_cluster:    db 1                    ; Sectors per cluster
bdb_reserved_sectors:       dw 1                    ; Reserved sectors
bdb_fat_count:              db 2                    ; Number of FATs
bdb_dir_entries_count:      dw 0E0h                 ; Max directory entries
bdb_total_sectors:          dw 2880                 ; Total sectors
bdb_media_descriptor_type:  db 0F0h                 ; Media descriptor
bdb_sectors_per_fat:        dw 9                    ; Sectors per FAT
bdb_sectors_per_track:      dw 18                   ; Sectors per track
bdb_heads:                  dw 2                    ; Number of heads
bdb_hidden_sectors:         dd 0                    ; Hidden sectors
bdb_large_sector_count:     dd 0                    ; Large sector count

; Extended Boot Record
ebr_drive_number:           db 0                    ; Drive number
                            db 0                    ; Reserved byte
ebr_signature:              db 29h                  ; Signature
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; Volume ID
ebr_volume_label:           db 'NANOBYTE OS'        ; Volume Label
ebr_system_id:              db 'FAT12   '           ; File system type

start:
	jmp main

;
; Function to output string to screen
; Outputs characters until a null terminator is encountered
; @param ds:si points to the string
;
puts:
	; Preserve registers
	push si
	push ax
	push bx 	

.output_loop
	lodsb		; Load next byte from DS:SI into AL, increment SI
	or al, al	; Check if the byte is null
	jz .done	; If null, jump to done

	mov ah, 0x0E	; BIOS teletype output
	mov bh, 0		; Page number 0
	int 0x10		; Call BIOS interrupt

	jmp .output_loop	; Continue loop

.done:
	; Restore registers
	pop bx
	pop ax
	pop si
	ret

main: 
	; Setup segment registers
	mov ax, 0
	mov ds, ax
	mov es, ax

	; Setup stack segment
	mov ss, ax
	mov sp, 0x7C00	; Initialize stack pointer

	; Save drive number
    mov [ebr_drive_number], dl

    ; Read second sector from floppy disk
    mov ax, 1                   ; LBA=1 (second sector)
    mov cl, 1                   ; Read 1 sector
    mov bx, 0x7E00              ; Load data after bootloader
    call disk_read

	; Display "Hello world" message
	mov si, msg_hello
	call puts

	cli                         ; Disable interrupts
	hlt

;
; Error Handling and Reboot
;

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah, 0
    int 16h                     ; Wait for key press
    jmp 0FFFFh:0                ; Reboot by jumping to BIOS start

.halt:
    cli                         ; Disable interrupts
    hlt                         ; Halt CPU

;
; Disk Operations
;

;
; Convert LBA to CHS
; @param ax: LBA address
; @return cx, dh
;
lba_to_chs:
    push ax
    push dx

    xor dx, dx                          ; Clear DX
    div word [bdb_sectors_per_track]    ; Divide LBA by sectors per track

    inc dx                              ; Increment DX for sector number
    mov cx, dx                          ; Store sector in CX

    xor dx, dx                          ; Clear DX again
    div word [bdb_heads]                ; Divide by number of heads
    mov dh, dl                          ; Head number
    mov ch, al                          ; Cylinder number (low)
    shl ah, 6
    or cl, ah                           ; Cylinder number (high bits)

    pop dx
    pop ax
    ret

;
; Read sectors from disk
; @param ax: LBA address
; @param cl: number of sectors to read
; @param dl: drive number
; @param es:bx: memory address for data
;
disk_read:
    push ax                             ; Save registers
    push bx
    push cx
    push dx
    push di

    push cx                             ; Save number of sectors to read
    call lba_to_chs                     ; Convert LBA to CHS
    pop ax                              ; Restore number of sectors

    mov ah, 02h                         ; BIOS read function
    mov di, 6                           ; Retry count

.retry:
    pusha                               ; Save all registers
    stc                                 ; Set carry flag
    int 13h                             ; BIOS interrupt for disk read
    jnc .done                           ; If no carry, read was successful

    ; Read failed, retry
    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
    ; All retries exhausted
    jmp floppy_error

.done:
    popa                                ; Restore all registers

    pop di
    pop dx
    pop cx
    pop bx
    pop ax                              ; Restore modified registers
    ret

;
; Reset disk controller
; @param dl: drive number
;
disk_reset:
    pusha
    mov ah, 0
    stc
    int 13h                             ; BIOS disk reset
    jc floppy_error
    popa
    ret

msg_hello: 				db 'Hello world from AjitOS!', ENDL, 0
msg_read_failed:        db 'Read from disk failed!', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h
