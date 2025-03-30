[global _start]
[extern kmain]

[section .text]

[bits 32]
; check if we booted using multiboot2.
check_multiboot:
	cmp eax, 0x36d76289
	jne hang
	ret

_start:
	; set up the stack.
	mov esp, stack_top
	
	; reset eflags.
	push 0
	popf

	; clear interrupts.
	cli
	
	; push multiboot structure.
	push ebx
	push eax
	
	; jump to kernel C code.
	jmp kmain
	
hang:
	cli
	hlt
	
	; if we wake up from an NMI, hang again.
	jmp hang
	
[section .bss]

; stack
stack_bottom:
	resb 4096 * 4
stack_top:
	align 16