#ifndef _CONFIG_H
#define _CONFIG_H

#include <lang.h>
#include <testcase.h>
#include <limits.h>

#define NUM_DOCKER_CONF		3
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

struct conf {
		enum lang lang;
		char filepath[PATH_MAX];
		struct docker_conf docker_conf;
		struct testcase *testcases;
};

int init_config(int argc, char **argv, struct conf *config);
int exec_cmd(char **argv, char *_stdin, char **_stdout);

#endif
