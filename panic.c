#include "lib.h"
#include "multiboot.h"
#include "kmalloc.h"

// asmbits.s
void *faulty_address();

void interrupt_handler_14() // page faults handler
{
	panic("page fault, unmapped address %p", faulty_address());
}

void interrupt_handler_13() // general protection fault handler
{
	panic("general protection fault");
}

struct frame
{
	struct frame *ebp;
	u32 eip;
};

void **symbols_addr;
char **symbols_names;
int has_symbols;
int nsymbols;

void sort_symbols()
{
	int i = 0;
	while (i < nsymbols - 1)
	{
		int j = 0;
		while (j < nsymbols - i - 1)
		{
			if ((addr)symbols_addr[j] > (addr)symbols_addr[j + 1])
			{
				// swap both addresses and names
				void *temp_addr = symbols_addr[j];
				symbols_addr[j] = symbols_addr[j + 1];
				symbols_addr[j + 1] = temp_addr;

				void *temp_name = symbols_names[j];
				symbols_names[j] = symbols_names[j + 1];
				symbols_names[j + 1] = temp_name;
			}
			j++;
		}
		i++;
	}
}

void load_symbols(struct multiboot_info *mboot_info)
{
	int i = 0;
	struct elf_shdr *shdr =
		(struct elf_shdr *)reloc(mboot_info->elf_sections.addr);
	while (i < mboot_info->elf_sections.num)
	{
		struct elf_shdr header = shdr[i];
		if (header.type == SHT_SYMTAB)
		{
			struct elf_sym *symtab = (struct elf_sym *)reloc(header.addr);
			char *strtab = (char *)reloc(shdr[header.link].addr);

			int j = 0;
			int symtab_size = header.size / sizeof(struct elf_sym);
			while (j < symtab_size)
				if (ELF_ST_TYPE(symtab[j++]) == STT_FUNC)
					nsymbols++;

			symbols_addr = kmalloc(nsymbols * sizeof(void *));
			symbols_names = kmalloc(nsymbols * sizeof(char *));
			if (symbols_addr == 0 || symbols_names == 0)
				return;

			j = 0;
			int k = 0;
			while (j < symtab_size)
			{
				if (ELF_ST_TYPE(symtab[j]) == STT_FUNC)
				{
					symbols_names[k] = strtab + symtab[j].name;
					symbols_addr[k] = (void *)symtab[j].value;
					k++;
				}
				j++;
			}
			has_symbols = 1;
		}
		i++;
	}

	if (has_symbols)
		sort_symbols();

	warn(has_symbols == 0, "failed to load symbol table. stacktraces will be less clear");
}

char *find_function_name(void *eip, int *diff)
{
	int i = 0;
	while (i < nsymbols - 1)
	{
		if (eip > symbols_addr[i] && eip < symbols_addr[i + 1])
		{
			*diff = eip - symbols_addr[i];
			return symbols_names[i];
		}
		i++;
	}
	return "(unknown)";
}

void stacktrace()
{
	printk("stack trace:");
	struct frame *frame = __builtin_frame_address(0);

	while (frame)
	{
		int diff = 0;
		char *name = find_function_name((void *)frame->eip, &diff);

		if (!frame->ebp)
			// why is this higher_half.hang according to nm?
			name = "_start";

		printk("  [%s+%x]", name, diff, frame->eip);
		frame = frame->ebp;
	}
}
