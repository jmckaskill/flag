#include <flag.h>
#include <str.h>
#include <stdlib.h>
#include <stdio.h>

enum flag_type {
	FLAG_BOOL,
	FLAG_INT,
	FLAG_DOUBLE,
	FLAG_STRING,
};

union all_types {
	bool b;
	const char *s;
	int i;
	double d;
};

struct flag {
	enum flag_type type;
	union all_types *pval;
	char shopt;
	const char *longopt;
	int longlen;
	const char *arg;
	const char *usage;
};

static const char *g_usage;
static const char *g_argv0;
static struct flag *g_flags;
static int g_num;
static int g_cap;

static int normal_exit(int code, const char *msg) {
	fputs(msg, stderr);
	exit(code);
	return 0;
}

flag_exit_fn flag_exit = &normal_exit;

static void print_spaces(str_t *o, int num) {
	while (num > 0) {
		str_addch(o, ' ');
		num--;
	}
}

static void print_usage(str_t *o) {
	str_addf(o, "usage: %s %s\n", g_argv0, g_usage);
	if (g_num) {
		str_add(o, "\noptions:\n");
		for (int i = 0; i < g_num; i++) {
			struct flag *f = &g_flags[i];
			int pad = 32;

			if (f->type == FLAG_BOOL) {
				if (f->shopt && f->longopt) {
					pad -= str_addf(o, "  -%c, --%s, --no-%s  ", f->shopt, f->longopt, f->longopt);
				} else if (f->longopt) {
					pad -= str_addf(o, "  --%s, --no-%s  ", f->longopt, f->longopt);
				} else {
					pad -= str_addf(o, "  -%c  ", f->shopt);
				}
			} else {
				if (f->shopt && f->longopt) {
					pad -= str_addf(o, "  -%c %s, --%s=%s  ", f->shopt, f->arg, f->longopt, f->arg);
				} else if (f->longopt) {
					pad -= str_addf(o, "  --%s=%s  ", f->longopt, f->arg);
				} else {
					pad -= str_addf(o, "  -%c %s  ", f->shopt, f->arg);
				}
			}

			print_spaces(o, pad);
			str_add(o, f->usage);

			switch (f->type) {
			case FLAG_INT:
				str_addf(o, " [default=%d]", f->pval->i);
				break;
			case FLAG_DOUBLE:
				str_addf(o, " [default=%g]", f->pval->d);
				break;
			case FLAG_STRING:
				if (f->pval->s) {
					str_addf(o, " [default=%s]", f->pval->s);
				}
				break;
			case FLAG_BOOL:
				if (f->pval->b) {
					str_addf(o, " [default=enabled]");
				}
				break;
			}

			str_addch(o, '\n');
		}
	}
}

int flag_error(int code, const char *fmt, ...) {
	str_t o = STR_INIT;
	va_list ap;
	va_start(ap, fmt);
	str_vaddf(&o, fmt, ap);
	if (o.len) {
		str_addch(&o, '\n');
	}
	print_usage(&o);
	int ret = flag_exit(code, o.c_str);
	str_destroy(&o);
	return ret;
}

static void append(enum flag_type type, void *p, char shopt, const char *longopt, const char *arg, const char *usage) {
	if (g_num == g_cap) {
		g_cap = (g_cap + 16) * 3 / 2;
		g_flags = (struct flag*) realloc(g_flags, g_cap * sizeof(*g_flags));
	}
	struct flag *f = &g_flags[g_num++];
	f->type = type;
	f->shopt = shopt;
	f->longopt = longopt;
	f->longlen = longopt ? strlen(longopt) : 0;
	f->arg = arg;
	f->usage = usage;
	f->pval = (union all_types *) p;
}

void flag_bool(bool *p, char shopt, const char *longopt, const char *usage) {
	append(FLAG_BOOL, p, shopt, longopt, NULL, usage);
}

void flag_int(int *p, char shopt, const char *longopt, const char *arg, const char *usage) {
	append(FLAG_INT, p, shopt, longopt, arg, usage);
}

void flag_double(double *p, char shopt, const char *longopt, const char *arg, const char *usage) {
	append(FLAG_DOUBLE, p, shopt, longopt, arg, usage);
}

void flag_string(const char **p, char shopt, const char *longopt, const char *arg, const char *usage) {
	append(FLAG_STRING, (void*) p, shopt, longopt, arg, usage);
}

static struct flag *find_long(const char *name, int len) {
	for (int j = 0; j < g_num; j++) {
		struct flag *f = &g_flags[j];
		if (f->longopt && len == f->longlen && !memcmp(name, f->longopt, len)) {
			return f;
		}
	}
	return NULL;
}

static struct flag *find_short(char name) {
	for (int j = 0; j < g_num; j++) {
		struct flag *f = &g_flags[j];
		if (f->shopt == name) {
			return f;
		}
	}
	return NULL;
}

static int process_flag(struct flag *f, char *arg, char *str_value, bool bool_value) {
	if (!str_value && f->type != FLAG_BOOL) {
		return flag_error(2, "expected value for %s", arg);
	}

	switch (f->type) {
	case FLAG_BOOL:
		f->pval->b = bool_value;
		break;
	case FLAG_INT:
		f->pval->i = strtol(str_value, NULL, 0);
		break;
	case FLAG_DOUBLE:
		f->pval->d = strtod(str_value, NULL);
		break;
	case FLAG_STRING:
		f->pval->s = str_value;
		break;
	}

	return 0;
}

static char *remove_argument(int i, int *pargc, char **argv) {
	char *ret = argv[i];
	memmove((void*) &argv[i], (void*) &argv[i + 1], (*pargc - (i + 1)) * sizeof(argv[i]));
	(*pargc)--;
	return ret;
}

static int unknown_flag(char *arg) {
	return flag_error(2, "unknown flag %s", arg);
}

int flag_parse(int *pargc, char **argv, const char *usage, int minargs) {
	g_usage = usage;
	g_argv0 = remove_argument(0, pargc, argv);
	for (int i = 0; i < *pargc;) {
		if (argv[i][0] != '-') {
			i++;
			continue;
		}

		char *arg = remove_argument(i, pargc, argv);

		if (!strcmp(arg, "--help") || !strcmp(arg, "-h")) {
			return flag_error(1, "");
		} else if (!strcmp(arg, "--")) {
			break;
		}

		int err;

		if (!strncmp(arg, "--no-", 5)) {
			// long form negative
			int len = strlen(arg);
			struct flag *f = find_long(arg + 5, len - 5);
			if (!f) {
				return unknown_flag(arg);
			}

			err = process_flag(f, arg, NULL, false);

		} else if (arg[1] == '-') {
			// long form
			int len = strlen(arg);
			char *value = memchr(arg, '=', len);
			if (value) {
				len = value - arg;
				value++;
			}

			struct flag *f = find_long(arg + 2, len - 2);
			if (!f) {
				return unknown_flag(arg);
			}

			err = process_flag(f, arg, value, true);

		} else if (arg[1] && !arg[2]) {
			// short form
			struct flag *f = find_short(arg[1]);
			if (!f) {
				return unknown_flag(arg);
			}

			char *value = NULL;
			if (f->type != FLAG_BOOL && i < *pargc) {
				value = remove_argument(i, pargc, argv);
			}

			err = process_flag(f, arg, value, true);
		} else {
			return unknown_flag(arg);
		}

		if (err) {
			return err;
		}
	}

	if (*pargc < minargs) {
		return flag_error(3, "expected %d arguments", minargs);
	}
	
	// re null-terminate the argument list
	argv[*pargc] = NULL;

	free(g_flags);
	g_flags = NULL;
	g_num = 0;
	g_cap = 0;
	return 0;
}

