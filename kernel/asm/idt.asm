[global idt_apply]
[global isr_common_stub]
[global irq_common_stub]

[extern isr_handler]
[extern irq_handler]

%macro xpusha 0
    push edi
    push esi
    push ebp
    push esp
    push ebx
    push edx
    push ecx
    push eax
%endmacro

%macro xpopa 0
    pop eax
    pop ecx
    pop edx
    pop ebx
    pop ebp
    pop ebp
    pop esi
    pop edi
%endmacro

idt_apply:
    mov eax, [esp + 4]
    lidt [eax]
    sti
    ret

isr_common_stub:
    pusha

    mov ax, ds
    push eax

    ; load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; call isr handler
    push esp
    call isr_handler
    pop ebx

    ; reload original data segment
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret

irq_common_stub:
    pusha

    mov ax, ds
    push eax

    ; load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; call irq handler
    push esp
    call irq_handler
    pop ebx

    ; reload original data segment
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret