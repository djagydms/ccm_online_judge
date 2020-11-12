#ifndef _CONFIG_H
#define _CONFIG_H

#include <lang.h>
#include <testcase.h>
#include <limits.h>

#define DEFAULT_CPUS		1
#define DEFAULT_MEMORY		536870912

struct docker_conf {
		char name[PATH_MAX];
		unsigned int cpus;
		unsigned int memory;
};

#define CONF_DELIMETER		":"
#define MARKING_IMAGE		"marking:0.2"
#define DOCKER_PATH			"/usr/bin/docker"

#define SEC_TO_NSEC			(long long int)1000000000
#define NSEC_TO_SEC			(long long int)1000000000

#define DEFAULT_COMPILE_LIMIT	(long long int)5000000000
#define DEFAULT_EXEC_LIMIT		(long long int)60000000000

struct conf {
		int lang;
		char filepath[PATH_MAX];
		struct docker_conf docker_conf;
		struct testcase *testcases;
		struct langsw langsw[MAX_LANG];
		long long int compile_limit;
		long long int exec_limit;
};

int init_config(int argc, char **argv, struct conf *config);
int exec_cmd(char **argv, char *_stdin, char **_stdout, long long int limit_time);

#endif
