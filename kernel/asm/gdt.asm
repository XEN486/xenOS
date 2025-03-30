[global gdt_apply]

gdt_apply:
    ; load the new GDT.
    mov eax, [esp + 4]
    lgdt [eax]

    ; data segment is at 0x10. set the segment registers.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; far jump to code segment at 0x08.
    jmp 0x08:.loaded

.loaded:
    ret