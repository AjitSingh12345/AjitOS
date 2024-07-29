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
    ; setup data segments
    mov ax, 0           ; can't set ds/es directly
    mov ds, ax
    mov es, ax
    
    ; setup stack
    mov ss, ax
    mov sp, 0x7C00              ; stack grows downwards from where we are loaded in memory

    ; some BIOSes might start us at 07C0:0000 instead of 0000:7C00, make sure we are in the
    ; expected location
    push es
    push word .after
    retf

.after:
    ; read something from floppy disk
    ; BIOS should set DL to drive number
    mov [ebr_drive_number], dl

    ; show loading message
    mov si, msg_loading
    call puts

    ; read drive parameters (sectors per track and head count),
    ; instead of relying on data on formatted disk
    push es
    mov ah, 08h
    int 13h
    jc floppy_error
    pop es

    and cl, 0x3F                        ; remove top 2 bits
    xor ch, ch
    mov [bdb_sectors_per_track], cx     ; sector count

    inc dh
    mov [bdb_heads], dh                 ; head count

    ; compute LBA of root directory = reserved + fats * sectors_per_fat
    ; note: this section can be hardcoded
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx                              ; ax = (fats * sectors_per_fat)
    add ax, [bdb_reserved_sectors]      ; ax = LBA of root directory
    push ax

    ; compute size of root directory = (32 * number_of_entries) / bytes_per_sector
    mov ax, [bdb_dir_entries_count]
    shl ax, 5                           ; ax *= 32
    xor dx, dx                          ; dx = 0
    div word [bdb_bytes_per_sector]     ; number of sectors we need to read

    test dx, dx                         ; if dx != 0, add 1
    jz .root_dir_after
    inc ax                              ; division remainder != 0, add 1
                                        ; this means we have a sector only partially filled with entries
.root_dir_after:

    ; read root directory
    mov cl, al                          ; cl = number of sectors to read = size of root directory
    pop ax                              ; ax = LBA of root directory
    mov dl, [ebr_drive_number]          ; dl = drive number (we saved it previously)
    mov bx, buffer                      ; es:bx = buffer
    call disk_read

    ; search for kernel.bin
    xor bx, bx
    mov di, buffer

.search_kernel:
    mov si, file_kernel_bin
    mov cx, 11                          ; compare up to 11 characters
    push di
    repe cmpsb
    pop di
    je .found_kernel

    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    ; kernel not found
    jmp kernel_not_found_error

.found_kernel:

    ; di should have the address to the entry
    mov ax, [di + 26]                   ; first logical cluster field (offset 26)
    mov [kernel_cluster], ax

    ; load FAT from disk into memory
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ; read kernel and process FAT chain
    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    
    ; Read next cluster
    mov ax, [kernel_cluster]
    
    ; not nice :( hardcoded value
    add ax, 31                          ; first cluster = (kernel_cluster - 2) * sectors_per_cluster + start_sector
                                        ; start sector = reserved + fats + root directory size = 1 + 18 + 134 = 33
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    ; compute location of next cluster
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                              ; ax = index of entry in FAT, dx = cluster mod 2

    mov si, buffer
    add si, ax
    mov ax, [ds:si]                     ; read entry from FAT table at index ax

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8                      ; end of chain
    jae .read_finish

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.read_finish:
    
    ; jump to our kernel
    mov dl, [ebr_drive_number]          ; boot device in dl

    mov ax, KERNEL_LOAD_SEGMENT         ; set segment registers
    mov ds, ax
    mov es, ax

    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

    jmp wait_key_and_reboot             ; should never happen

    cli                                 ; disable interrupts, this way CPU can't get out of "halt" state
    hlt


;
; Error handlers
;

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

kernel_not_found_error:
    mov si, msg_kernel_not_found
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah, 0
    int 16h                     ; wait for keypress
    jmp 0FFFFh:0                ; jump to beginning of BIOS, should reboot

.halt:
    cli                         ; disable interrupts, this way CPU can't get out of "halt" state
    hlt


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

.output_loop:
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

;
; Disk Operations
;

;
; Convert LBA to CHS
; @param ax: LBA address
; @return:
;   - cx [bits 0-5]: sector number
;   - cx [bits 6-15]: cylinder
;   - dh: head
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

    pop ax
    mov dl, al                          ; restore DL
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

msg_loading:            db 'Loading...', ENDL, 0
msg_read_failed:        db 'Read from disk failed!', ENDL, 0
msg_kernel_not_found:   db 'KERNEL.BIN file not found!', ENDL, 0
file_kernel_bin:        db 'KERNEL  BIN'
kernel_cluster:         dw 0

KERNEL_LOAD_SEGMENT     equ 0x2000
KERNEL_LOAD_OFFSET      equ 0

times 510-($-$$) db 0
dw 0AA55h

buffer:
