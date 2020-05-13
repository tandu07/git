#include "test-tool.h"
#include "git-compat-util.h"
#include "gettext.h"

struct reg_flag {
	const char *name;
	int flag;
};

static struct reg_flag reg_flags[] = {
	{ "EXTENDED",	 REG_EXTENDED	},
	{ "NEWLINE",	 REG_NEWLINE	},
	{ "ICASE",	 REG_ICASE	},
	{ "NOTBOL",	 REG_NOTBOL	},
#ifdef REG_STARTEND
	{ "STARTEND",	 REG_STARTEND	},
#endif
	{ NULL, 0 }
};

static int test_regex_bug(void)
{
	char *pat = "[^={} \t]+";
	char *str = "={}\nfred";
	regex_t r;
	regmatch_t m[1];

	if (regcomp(&r, pat, REG_EXTENDED | REG_NEWLINE))
		die("failed regcomp() for pattern '%s'", pat);
	if (regexec(&r, str, 1, m, 0))
		die("no match of pattern '%s' to string '%s'", pat, str);

	/* http://sourceware.org/bugzilla/show_bug.cgi?id=3957  */
	if (m[0].rm_so == 3) /* matches '\n' when it should not */
		die("regex bug confirmed: re-build git with NO_REGEX=1");

	return 0;
}

int cmd__regex(int argc, const char **argv)
{
	const char *pat;
	const char *str;
	int ret, silent = 0, flags = 0;
	regex_t r;
	regmatch_t m[1];
	char errbuf[64];

	if (argc == 2 && !strcmp(argv[1], "--bug"))
		return test_regex_bug();
	else if (argc < 3)
		usage("test-tool regex --bug\n"
		      "test-tool regex [--silent] <pattern> <string> [<options>]");

	if (!strcmp(argv[1], "--silent")) {
		silent = 1;
		argv++;
	}
	argv++;
	pat = *argv++;
	str = *argv++;
	while (*argv) {
		struct reg_flag *rf;
		for (rf = reg_flags; rf->name; rf++)
			if (!strcmp(*argv, rf->name)) {
				flags |= rf->flag;
				break;
			}
		if (!rf->name)
			die("do not recognize %s", *argv);
		argv++;
	}
	git_setup_gettext();

	ret = regcomp(&r, pat, flags);
	if (ret) {
		if (!silent) {
			regerror(ret, &r, errbuf, sizeof(errbuf));
			die("failed regcomp() for pattern '%s' (%s)",
				pat, errbuf);
		} else
			return 1;
	}
	if (regexec(&r, str, 1, m, 0))
		return 1;

	return 0;
}
