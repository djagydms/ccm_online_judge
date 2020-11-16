#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <conf.h>
#include <testcase.h>
#include <limits.h>

int python_prepare(void *_conf)
{
		struct conf *conf = _conf;
		char *args[5] = {DOCKER_PATH, "cp", conf->filepath,};
		char buffer[PATH_MAX * 2];

		sprintf(buffer, "%s:marking", conf->docker_conf.name);
		args[3] = buffer;
		args[4] = NULL;

		return exec_cmd(args, NULL, NULL, 0);
}

int python_exec(void *_conf, struct score *score)
{
		struct conf *conf = _conf;
		struct testcase *testcase = conf->testcases;
		char *args[7] = {DOCKER_PATH, "exec", "-i", 
				conf->docker_conf.name, "python3", conf->filename, NULL};
		struct timeval start_time, end_time;
		char *ret_ans;
		int numans = 0;

		fprintf(stderr, "%s\n", conf->filename);

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

struct langsw python_install()
{
		struct langsw sw = {
				.name = "PYTHON",
				.ext = ".py",
				.prepare = python_prepare,
				.exec = python_exec
		};

		return sw;
}
