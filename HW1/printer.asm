; printer.asm


print_newline:
    mov ah, 0x0E
    mov al, 0x0A
    int 0x10

    mov al, 0x0D
    int 0x10
    ret

print_hex:
    mov ah, 0x0E
    push cx

    mov cx, dx

    mov al, 0x30
    int 0x10

    mov al, 0x78
    int 0x10


    shr dx, 0xc
    call determnie_char

    mov dx, cx
    shr dx, 0x8
    and dl, 0x0f
    call determnie_char

    mov dx, cx
    shr dx, 0x4
    and dl, 0x0f
    call determnie_char

    mov dx, cx
    and dl, 0x0f
    call determnie_char

    pop cx
    ret
    
determnie_char:
    cmp dl, 0xA
    jge A
    jmp Ni

    A:
        call conver_ab_A_and_print
        ret

    Ni: 
        call conver_bl_9_and_print
        ret

    ret

conver_bl_9_and_print:
    add dl, 0x30

    mov al, dl
    int 0x10

    ret


conver_ab_A_and_print:
    add dl, 0x37

    mov al, dl
    int 0x10

    ret

