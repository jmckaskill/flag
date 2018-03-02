#include <flag.h>
#include <str.h>
#include <test.h>

static str_t g_lastmsg = STR_INIT;

static int my_exit(int code, const char *msg) {
	str_set(&g_lastmsg, msg);
	return code;
}

int main(int argc, char *argv[]) {
	start_test(&argc, argv, 10);

	bool b = false;
	int i = 34;
	const char *str = "default";

	flag_exit = &my_exit;

	// test the help
	flag_bool(&b, 'b', "bool", "bool usage");
	flag_int(&i, 0, "int", "N", "int usage");
	flag_string(&str, 's', NULL, "S", "string usage");

	char *args1[] = { strdup("foo"), strdup("-h"), NULL };
	int argc1 = 2;
	EXPECT_EQ(1, flag_parse(&argc1, args1, "[options]", 0));
	EXPECT_STREQ(
		"usage: foo [options]\n"
		"\n"
		"options:\n"
		"  -b, --bool, --no-bool         bool usage\n"
		"  --int=N                       int usage [default=34]\n"
		"  -s S                          string usage [default=default]\n",
		g_lastmsg.c_str);

	// test the parsing
	flag_bool(&b, 'b', "bool", "bool usage");
	flag_int(&i, 0, "int", "N", "int usage");
	flag_string(&str, 's', NULL, "S", "string usage");

	char *args2[] = { strdup("foo"), strdup("-b"), strdup("--int=3"), strdup("argument"), strdup("-s"), strdup("foobar"), NULL };
	int argc2 = 6;
	EXPECT_EQ(0, flag_parse(&argc2, args2, "[arguments]", 0));
	EXPECT_EQ(1, argc2);
	EXPECT_STREQ("argument", args2[0]);
	EXPECT_EQ(true, b);
	EXPECT_EQ(i, 3);
	EXPECT_STREQ(str, "foobar");

	return finish_test();
}
