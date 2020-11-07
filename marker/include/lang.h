#ifndef _LANG_H
#define _LANG_H

#include <testcase.h>

#define MAX_LANG		2

enum lang {
		C = 0,
		PYTHON
};

int c_create(void*);
int c_prepare(void*);
int c_exec(void*, struct score*);

int python_create(void*);
int python_prepare(void*);
int python_exec(void*, struct score*);

struct langsw {
		const char *name;
		const char *ext;
		int (*create)(void*);
		int (*prepare)(void*);
		int (*exec)(void*, struct score*); 
};

static struct langsw langsw[] = {
		{"C", ".c", c_create, c_prepare, c_exec},
		{"PYTHON", ".py", python_create, python_prepare, python_exec}
};

#endif
