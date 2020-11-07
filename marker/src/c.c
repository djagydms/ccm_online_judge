#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <conf.h>
#include <testcase.h>
#include <limits.h>

int c_create(void *_conf)
{
		struct conf *conf = _conf;
		char *args[9] = {DOCKER_PATH, "run", "-t", "-d",};
		char name[PATH_MAX * 2];
		char cpus[32];
		char memory[32];

		sprintf(name, "--name=%s", conf->docker_conf.name);
		sprintf(cpus, "--cpus=%d", conf->docker_conf.cpus);
		sprintf(memory, "--memory=%d", conf->docker_conf.memory);

		args[4] = name;
		args[5] = cpus;
		args[6] = memory;
		args[7] = MARKING_IMAGE;
		args[8] = NULL;

		return exec_cmd(args, NULL, NULL);
}

int c_prepare(void *_conf)
{
		struct conf *conf = _conf;
		char *args[5] = {DOCKER_PATH, "cp", conf->filepath,};
		char *args2[] = {DOCKER_PATH, "exec", 
				conf->docker_conf.name, "gcc", conf->filepath, NULL};
		char buffer[PATH_MAX * 2];
		int ret;

		sprintf(buffer, "%s:/", conf->docker_conf.name);
		args[3] = buffer;
		args[4] = NULL;

		if (ret = exec_cmd(args, NULL, NULL))
				return ret;

		return exec_cmd(args2, NULL, NULL);
}

int c_exec(void *_conf, struct score *score)
{
		struct conf *conf = _conf;
		struct testcase *testcase = conf->testcases;
		char *args[6] = {DOCKER_PATH, "exec", "-i", 
				conf->docker_conf.name, "./a.out", NULL};
		struct timeval start_time, end_time;
		char *ret_ans;
		int numans = 0;

		ret_ans = malloc(65536);
		if (ret_ans == NULL)
				return -ENOMEM;

		score->exectime = 0;
		while (testcase != NULL) {
				gettimeofday(&start_time, NULL);
				exec_cmd(args, testcase->_case, &ret_ans);
				gettimeofday(&end_time, NULL);

				score->exectime += end_time.tv_usec > start_time.tv_usec ? 
						end_time.tv_usec - start_time.tv_usec : 0;

				score->marking[numans++] =
						strcmp(testcase->_ans, ret_ans) ? 'x' : 'o';
				assert(numans < MAX_CASE);

				testcase = testcase->next;
		}
		free(ret_ans);

		return 0;
}
