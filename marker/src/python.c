#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <conf.h>
#include <sys/time.h>
#include <string.h>

int python_create(void *conf)
{
		struct docker_conf *config = conf;
		char cpus[32];
		char memory[32];
		char *args[9] = {DOCKER_PATH, "run", "-t", "-d",};

		/* TODO: remove this */
		char *args1[5] = {DOCKER_PATH, "rm", "-f", "test", NULL};
		exec_cmd(args1, NULL, NULL);

		sprintf(cpus, "--cpus=%d", config->cpus);
		sprintf(memory, "--memory=%d", config->memory);
		args[4] = cpus;
		args[5] = memory;
		args[6] = "--name=test";
		args[7] = MARKING_IMAGE;
		args[8] = NULL;

		/* /usr/bin/docker run -t -d --cpus=1 --memory=5~~ --name=test marking:0.2 */

		return exec_cmd(args, NULL, NULL);
}

int python_prepare(char *filepath)
{
		char *args[] = {DOCKER_PATH, "cp", filepath, "test:/", NULL};

		return exec_cmd(args, NULL, NULL);
}

char *python_exec(void *head)
{
		struct conf *config = head;
		struct testcase *testcase = config->testcases;
		char *args[7] = {DOCKER_PATH, "exec", "-i", "test", "python3", config->file_path, NULL};
		struct timeval start_time, end_time;
		char *ret_json;
		char *ret_ans;
		int numans = 0;
		long exec_time = 0;
		int marking[65536];

		ret_ans = malloc(65536);
		if (ret_ans == NULL) {
				return NULL;
		}

		while (testcase != NULL) {
				gettimeofday(&start_time, NULL);
				exec_cmd(args, testcase->_case, &ret_ans);
				gettimeofday(&end_time, NULL);

				exec_time += end_time.tv_usec > start_time.tv_usec ? 
						end_time.tv_usec - start_time.tv_usec : 0;

				if (ret_ans[strlen(ret_ans) - 1] == '\n' ||
					ret_ans[strlen(ret_ans) - 1] == '\r')
					ret_ans[strlen(ret_ans) - 1]  = '\0';

				if (!strcmp(testcase->_ans, ret_ans)) {
						marking[numans++] = 1;
				} else {
						marking[numans++] = 0;
				}

				testcase = testcase->next;
		}

		for (int i=0; i<numans; i++) {
				ret_ans[i] = marking[i] ? 'o' : 'x';
		}
		ret_ans[numans] = '\0';

		ret_json = malloc(sizeof(65536));
		if (ret_json > 0) {
				sprintf(ret_json,
						"{\"marking\":\"%s\",\"exectime\":%ld}", 
						ret_ans, exec_time);
		}
		free(ret_ans);

		return ret_json;
}
