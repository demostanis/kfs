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

	; push stack frame containing the faulty function
	; 36 = sizeof(usize) * (8 (n of registers pushed to the stack
	; by pushad) + 1)
	push dword [esp + 36]
	push ebp

	mov ebp, esp ; save stack

	push dword [esp + 64] ; emit_signal() argument

	call interrupt_handler_%1

	add esp, 12 ; remove from the stack everything we pushed after `pushad`
	popad
	iret
%endmacro

; hardware interrupts
isr 9 ; keyboard
isr 14 ; page fault
isr 13 ; general protection fault

; software interrupts
isr 28 ; line received

export load_idt:
	extern idtp ; idt.c

	lidt [idtp]
	sti
	ret

export faulty_address:
	mov eax, cr2
	ret

%define LINE 28 ; idt.h
export emit_signal:
	cmp eax, LINE
	jne .end
	push ebx ; TODO: push more registers
	int 28
	pop ebx
.end:
	ret
