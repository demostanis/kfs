export gdt_flush:
	extern gp ; gdt.c

	lgdt [gp]
	mov ax, 0x10 ; https://stackoverflow.com/questions/23978486/far-jump-in-gdt-in-bootloader
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:.flush
.flush:
	ret

export shutdown:
	; only works on QEMU
	mov dx, 0x604
	mov ax, 0x2000
	out dx, ax
	jmp shutdown ; sometimes it needs to be done multiple times...

export load_page_directory:
	mov cr3, eax
	ret

export enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret

%macro isr 1
export isr_wrapper_%1:
	extern interrupt_handler_%1

	; tcc doesn't support __attribute__((interrupt)) ;(
	pushad
	cld
	call interrupt_handler_%1
	popad
	iret
%endmacro

isr 9 ; keyboard
isr 14 ; page fault

export load_idt:
	extern idtp ; idt.c

	lidt [idtp]
	sti
	ret

export faulty_address:
	mov eax, cr2
	ret
