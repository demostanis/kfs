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
