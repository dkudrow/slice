#include <string.h>
#include <util.h>

#include "test.h"

const char *test_name = "KPRINTF";

const char *run_test()
{
	char *cons = dummy_console_reset();
	kprintf("%u", 0);
	if (strncmp(cons, "0", 100))
		return "formatting 0 as '%u'";

	dummy_console_reset();
	kprintf("%u", 1234567890);
	if (strncmp(cons, "1234567890", 100))
		return "formatting 1234567890 as '%u'";

	dummy_console_reset();
	kprintf("%x", 0);
	if (strncmp(cons, "0", 100))
		return "formatting 0x0 as '%x'";

	dummy_console_reset();
	kprintf("%x", 0xDEADBEEF);
	if (strncmp(cons, "DEADBEEF", 100))
		return "formatting 0xDEADBEEF as '%x'";

	dummy_console_reset();
	kprintf("%i", 0);
	if (strncmp(cons, "0", 100))
		return "formatting 0 as '%i'";

	dummy_console_reset();
	kprintf("%i", -1234567890);
	if (strncmp(cons, "-1234567890", 100))
		return "formatting -1234567890 as '%i'";

	return NULL;
}
