#ifndef _CONFIG_H
#define _CONFIG_H

#include <lang.h>

struct testcase {
		char *_case;
		char *_ans;
		struct testcase *next;
};

#define DEFAULT_CPUS		1
#define DEFAULT_MEMORY		536870912

struct docker_conf {
		unsigned int cpus;
		unsigned int memory;
};

#define NUM_DOCKER_CONF		2
#define CONF_DELIMETER		":"
#define MARKING_IMAGE		"marking:0.2"
#define DOCKER_PATH			"/usr/bin/docker"

struct conf {
		enum lang lang;
		char *file_path;
		struct docker_conf docker_conf;
		struct testcase *testcases;
};

int init_config(int, char**, struct conf*);
int exec_cmd(char **, char *, char **);

#endif
