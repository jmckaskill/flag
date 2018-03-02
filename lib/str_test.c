#include <str.h>
#include <test.h>

int main(int argc, char *argv[]) {
	start_test(&argc, argv, 10);

	str_t s = STR_INIT;
	str_set(&s, "foo bar foo bar");
	str_replace_all(&s, "foo", "abcde");
	EXPECT_STREQ("abcde bar abcde bar", s.c_str);
	str_replace_all(&s, "bar", "a");
	EXPECT_STREQ("abcde a abcde a", s.c_str);
	str_replace_all(&s, "abcde", "ghijk");
	EXPECT_STREQ("ghijk a ghijk a", s.c_str);

	return finish_test();
}
