; all constants taken from
; https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html
%define MULTIBOOT_HEADER_MAGIC 0x1BADB002
%define MULTIBOOT_PAGE_ALIGN 0x00000001
%define MULTIBOOT_FLAGS (MULTIBOOT_PAGE_ALIGN)

%macro export 1
	global %1 ; func
	%1
%endmacro

section .multiboot
align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_FLAGS
	dd -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_FLAGS)

section .bss
       resb 16*1024 ; reserve 16KiB stack
stack:

section .multiboot.text
export _start:
	extern page_directory
	extern page_table0
	extern kernel_begin
	extern kernel_end

	mov edi, page_table0
	mov esi, 0

.map_page:
	cmp esi, kernel_end ; mapped the entire kernel yet?
	jge .setup_paging

	; add page table entry
	mov edx, esi
	or edx, 3 ; writable, present
	mov [edi], edx

	add esi, 4096 ; move to the next page
	add edi, 4 ; next table entry
	jmp .map_page

.setup_paging:
	; our table is the page directory's 768th entry (i.e. 0xc0000000)
	mov dword [page_directory + 768 * 4], page_table0 + 3
	; also map at the beginning of the memory, so the
	; CPU can still find the instructions after enabling
	; paging (i.e. call higher_half). note that this is
	; temporary, the actual page directory in C don't
	; do this, else it wouldn't make sense to map the kernel
	; in the higher half...
	mov dword [page_directory], page_table0 + 3
	mov ecx, page_directory
	mov cr3, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	call higher_half

section .text
higher_half:
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
