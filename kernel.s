section .multiboot
align 4
	dd 0x1BADB002  ; magic
	dd 0           ; flags
	dd -0x1BADB002 ; checksum

section .text
global _start
_start:
	extern kmain
	call kmain

	hlt

global gdt_flush
extern gp
gdt_flush:
	lgdt [gp]
	mov ax, 0x10 ; https://stackoverflow.com/questions/23978486/far-jump-in-gdt-in-bootloader
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:flush
flush:
	ret
