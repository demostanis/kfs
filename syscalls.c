#include "lib.h"

#define fake_syscall_n(n) \
	void syscall_##n() { printk("executing syscall "#n); }

fake_syscall_n(0)
fake_syscall_n(1)
fake_syscall_n(2)

// example syscalls
void (*syscall_map[])() = {
	syscall_0,
	syscall_1,
	syscall_2,
};

void interrupt_handler_0x80(int syscalln) // syscall interrupt handler
{
	if (syscalln > len(syscall_map))
		return;
	syscall_map[syscalln]();
}
