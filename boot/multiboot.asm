[section .multiboot]
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    
    ; end tag
    align 8
    dw 0
    dw 0
    dd 8

header_end: