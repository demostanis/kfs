; all constants taken from
; https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html
%define MULTIBOOT_HEADER_MAGIC 0x1BADB002
%define MULTIBOOT_PAGE_ALIGN 0x00000001
%define MULTIBOOT_FLAGS (MULTIBOOT_PAGE_ALIGN)

section .multiboot
align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_FLAGS
	dd -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_FLAGS)

section .bss
align 16
       resb 16*1024 ; reserve 16KiB stack
stack:

section .text
global _start
_start:
	; kmain.c
	extern kcommon
	extern ktest
	extern kmain

	; setup stack pointer
	mov esp, stack

	push ebx ; magic
	push eax ; struct multiboot_info *

	call kcommon

%ifdef RUNTESTS
	call ktest
	call shutdown
%else
	call kmain
	hlt
%endif

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

global shutdown
shutdown:
	mov dx, 0x604
	mov ax, 0x2000
	out dx, ax
	jmp shutdown ; sometimes it needs to be done multiple times...
