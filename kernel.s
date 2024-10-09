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

%include "asmbits.s"
