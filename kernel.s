section .multiboot
align 4
	dd 0x1BADB002  ; magic
	dd 0           ; flags
	dd -0x1BADB002 ; checksum

section .bss
align 16
	resb 16*1024 ; reserve 16KiB stack
stack:

section .text
global _start
_start:
	; setup stack pointer
	mov esp, stack

	extern kmain
	call kmain

	hlt
