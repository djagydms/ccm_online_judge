#ifndef _LANG_H
#define _LANG_H

#include <testcase.h>

#define MAX_LANG		50

struct langsw {
		const char *name;
		const char *ext;
		int (*prepare)(void*);
		int (*exec)(void*, struct score*); 
};

struct langsw c_install();
struct langsw python_install();
static struct langsw (*install[MAX_LANG])(void) = {
		c_install,
		python_install,
		NULL
};

#endif
