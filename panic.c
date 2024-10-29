#include "lib.h"

// asmbits.s
void *faulty_address();

void interrupt_handler_14() // page faults handler
{
	panic("page fault, unmapped address %p", faulty_address());
}

struct frame
{
	struct frame *ebp;
	u32 eip;
};

void stacktrace()
{
	printk("stack trace:");
	struct frame *frame = __builtin_frame_address(0);

	while (frame)
	{
		printk("  %p", frame->eip);
		frame = frame->ebp;
	}
}
