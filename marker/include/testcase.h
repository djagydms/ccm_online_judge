#ifndef _TESTCASE_H
#define _TESTCASE_H

#define MAX_CASE		30 + 1

struct testcase {
		char *_case;
		char *_ans;
		struct testcase *next;
};

struct score {
		int status;
		char marking[MAX_CASE];
		long exectime;
};

void free_testcases(struct testcase *head);

#endif
