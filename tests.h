#pragma once

#include "serial.h"

/* asks the tester (which reads from the serial)
 * to ensure this line is equal to the previous */
#define TEST_OUT_CHECK "\2"

#define fn_name_i(f) ktest_##f
#define fn_name(f) fn_name_i(f)
#define TESTS() void fn_name(__FILENAME__)() { \
	write_serial_str("begin tests\n");
#define ENDTESTS write_serial_str("end tests\n");}

/* asserts the previous printk wrote `out` */
#define assert_serial(out) \
	write_serial_str(TEST_OUT_CHECK out "\n")
