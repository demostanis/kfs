#pragma once

#include "serial.h"

#define begin_test() { write_serial_str("begin tests\n"); }
#define end_test() { write_serial_str("end tests\n"); }

/* asks the tester (which reads from the serial)
 * to ensure this line is equal to the previous */
#define TEST_OUT_CHECK "\2"

#define fn_name_i(f) ktest_##f
#define fn_name(f) fn_name_i(f)
#define TESTS() void fn_name(__FILENAME__)()

/* asserts the previous printk wrote `out` */
#define assert_serial(out) \
	write_serial_str(TEST_OUT_CHECK out "\n")
