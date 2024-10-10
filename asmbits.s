global gdt_flush
gdt_flush:
	extern gp ; gdt.c

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
	; only works on QEMU
	mov dx, 0x604
	mov ax, 0x2000
	out dx, ax
	jmp shutdown ; sometimes it needs to be done multiple times...

global load_page_directory
load_page_directory:
	mov cr3, eax
	ret

global enable_paging
enable_paging:
	mov eax, cr0
	xor ebx, ebx
	shl ebx, 35
	or eax, ebx
	mov cr0, eax
	ret
