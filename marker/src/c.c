#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <conf.h>
#include <testcase.h>
#include <limits.h>

int c_prepare(void *_conf)
{
		struct conf *conf = _conf;
		char *args[5] = {DOCKER_PATH, "cp", conf->filepath,};
		char *args1[] = {DOCKER_PATH, "exec", 
				conf->docker_conf.name, "gcc", conf->filename, NULL};
		char buffer[PATH_MAX * 2];
		int ret;

		sprintf(buffer, "%s:marking", conf->docker_conf.name);
		args[3] = buffer;
		args[4] = NULL;

		if (ret = exec_cmd(args, NULL, NULL, 0))
				return ret;

		return exec_cmd(args1, NULL, NULL, conf->compile_limit);
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
				score->status = exec_cmd(args, testcase->_case, &ret_ans, conf->exec_limit);
				gettimeofday(&end_time, NULL);

				score->exectime += end_time.tv_usec > start_time.tv_usec ? 
						end_time.tv_usec - start_time.tv_usec : 0;

				if (score->exectime > conf->exec_limit)
						score->status = -ETIMEDOUT;

				if (score->status)
						break;

				if (ret_ans[strlen(ret_ans)-1] == '\n' ||
								ret_ans[strlen(ret_ans)-1] == '\r')
						ret_ans[strlen(ret_ans)-1] = '\0';

				score->marking[numans++] =
						strcmp(testcase->_ans, ret_ans) ? 'x' : 'o';
				assert(numans < MAX_CASE);

				testcase = testcase->next;
		}
		free(ret_ans);

		return 0;
}

struct langsw c_install()
{
		struct langsw sw = {
				.name = "C",
				.ext = ".c",
				.prepare = c_prepare,
				.exec = c_exec
		};

		return sw;
}
