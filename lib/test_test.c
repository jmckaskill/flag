#include "test.h"
#include "flag.h"
#include "str.h"
#include <math.h>
#include <stdlib.h>

static str_t last_msg = STR_INIT;

static int failure_cb(const char *file, int line, const char *msg) {
	str_set(&last_msg, msg);
	return 0;
}

static void check_msg(const char *fmt, ...) {
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	if (strcmp(last_msg.c_str, buf)) {
		fprintf(stderr, "expected:\n%sgot:\n%s\n", buf, last_msg.c_str);
		exit(2);
	}
	str_clear(&last_msg);
}

int main(int argc, char *argv[]) {
	const char *output_fn;
	flag_string(&output_fn, 'o', "output", "FILE", "output filename");
	flag_parse(&argc, argv, "[arguments]", 0);

	test_failed = &failure_cb;

	EXPECT_EQ(0, 0);
	check_msg("");
	EXPECT_EQ(0, 1);
	check_msg("value of 1\n\t  Actual: 1\n\tExpected: 0\n");
	EXPECT_EQ(0+0, 1);
	check_msg("value of 1\n\t  Actual: 1\n\tExpected: 0 (0+0)\n");

	EXPECT_FLOAT_EQ(0.2, 1.0);
	check_msg("value of 1.0\n\t  Actual: 1\n\tExpected: 0.2\n");
	EXPECT_FLOAT_EQ(NAN, 0);
	check_msg("value of 0\n\t  Actual: 0\n\tExpected: %g (NAN)\n", NAN);
	EXPECT_FLOAT_EQ(NAN, NAN);
	check_msg("");
	EXPECT_FLOAT_EQ(INFINITY, INFINITY);
	check_msg("");
	EXPECT_FLOAT_EQ(INFINITY+0, -INFINITY);
	check_msg("value of -INFINITY\n\t  Actual: %g\n\tExpected: %g (INFINITY+0)\n", -INFINITY, INFINITY);

	EXPECT_NEAR(0.2, 0.21, 0.01);
	check_msg("");
	EXPECT_NEAR(0.2, 0.21f, 0.001);
	check_msg("value of 0.21f\n\t  Actual: 0.21\n\tExpected: 0.2\n\t  Within: 0.001\n");

	char a, b;
	EXPECT_PTREQ(&a, &a);
	check_msg("");
	EXPECT_PTREQ(&a, &b);
	check_msg("value of &b\n\t  Actual: %p\n\tExpected: %p (&a)\n", &b, &a);

	EXPECT_STREQ("a", "a");
	check_msg("");
	EXPECT_STREQ("a", "ab");
	check_msg("value of \"ab\"\n\t  Actual: \"ab\"\n\tExpected: \"a\" (\"a\")\n");

	EXPECT_GT(1, 0);
	check_msg("");
	EXPECT_GT(0+0, 0-0);
	check_msg("expected 0 (0+0) > 0 (0-0)\n");
	EXPECT_GT(0-1, 0);
	check_msg("expected -1 (0-1) > 0\n");

	EXPECT_GE(1, 0);
	check_msg("");
	EXPECT_GE(0, 0);
	check_msg("");
	EXPECT_GE(0-1, 0);
	check_msg("expected -1 (0-1) >= 0\n");

	char ab[] = "ab";
	char bc[] = "bc";
	char abd[] = "abd";
	EXPECT_BYTES_EQ(ab, 2, abd, 2);
	check_msg("");
	EXPECT_BYTES_EQ(ab, 2, bc, 2);
	check_msg("value of bc\n\t  Actual: [x62,x63]\n\tExpected: [x61,x62] (ab)\n");
	EXPECT_BYTES_EQ(bc, 2, abd+1, 2);
	check_msg("value of abd+1\n\t  Actual: [x62,x64]\n\tExpected: [x62,x63] (bc)\n");
	EXPECT_BYTES_EQ(ab, 2, abd, 3);
	check_msg("value of abd\n\t  Actual: [x61,x62,x64]\n\tExpected: [x61,x62] (ab)\n");

	if (output_fn) {
		FILE *f = fopen(output_fn, "wb");
		if (f) {
			fclose(f);
		}
	}
	return 0;
}

