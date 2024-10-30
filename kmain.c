#include "lib.h"
#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "pmem.h"
#include "vmem.h"
#include "keyboard.h"
#include "shell.h"
#include "tests.h"

struct multiboot_info *mboot_info;

void kcommon(int magic, struct multiboot_info *info)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("kernel needs to be booted from multiboot1");

	if (info->flags & MULTIBOOT_INFO_MEM_MAP == 0)
		panic("missing memory map");

	if (info->flags & MULTIBOOT_INFO_ELF_SHDR == 0)
		panic("missing ELF sections");

	mboot_info = skip_bytes(info, UNUSED_FIELDS_PADDING);

	init_pmem_regions();
	init_page_directory();

	gdt_install();
	idt_install();
	clear_video();
	disable_cursor();

	// causes a GPF since there's no ISR for interrupt 69:
	// (for testing)
	//__asm__ volatile("int $0x69");

	// TODO: why does it say page already mapped?
	unmap_page((addr)reloc(mboot_info));
	map_phys_to_virt((addr)mboot_info, (addr)reloc(mboot_info));
	load_symbols(reloc(mboot_info));
}

void kmain()
{
	init_shell();
	// to test fake syscalls:
	//emit_signal(SYSCALL, (void *)1);
	//emit_signal(SYSCALL, (void *)0);
	//emit_signal(SYSCALL, (void *)23);
}
