;Main of HM1

;initlize data segment
mov ax, 0x7C0 
mov ds, ax

;initlize stack segment
mov ax, 0x7E0 
mov ss, ax

; init stack
mov bp, 0x800
mov sp, bp

mov ah, 0x0e

mov al, 'H'
int 0x10

mov al, 'e'
int 0x10

mov al, 'l'
int 0x10

mov al, 'l'
int 0x10

mov al, 'o'
int 0x10

call print_newline

mov dx, 0xF32a
call print_hex

jmp $

%include "printer.asm"

times 510 - ($ - $$) db 0

dw 0xaa55


