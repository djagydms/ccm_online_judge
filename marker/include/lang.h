#ifndef _LANG_H
#define _LANG_H

#define MAX_LANG		4

enum lang {
		C = 0,
		CPP,
		JAVA,
		PYTHON
};

int 	c_create(void*);
int 	c_prepare(char*);
char *	c_exec(void*);
int 	cpp_create(void*);
int 	cpp_prepare(char*);
char *	cpp_exec(void*);
int 	java_create(void*);
int 	java_prepare(char*);
char *	java_exec(void*);
int 	python_create(void*);
int 	python_prepare(char*);
char *	python_exec(void*);

struct langsw {
		const char *name;
		int (*create)(void*);
		int (*prepare)(char*);
		char *(*exec)(void*);
};

static struct langsw langsw[] = {
		{"C", c_create, c_prepare, c_exec},
		{"CPP", cpp_create, cpp_prepare, cpp_exec},
		{"JAVA", java_create, java_prepare, java_exec},
		{"PYTHON", python_create, python_prepare, python_exec}
};

#endif
