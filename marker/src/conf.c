#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <conf.h>
#include <wait.h>
#include <unistd.h>

enum lang __select_lang(char *name)
{
		enum lang i;
		for (i = C; langsw[i].name != NULL; i++) {
				if (!strcmp(langsw[i].name, name)) {
						return i;
				}
		}

		return -ENODATA;
}

struct docker_conf __set_docker_conf(char *docker_confs)
{
		struct docker_conf config;
		char *ptr;

		ptr = strtok(docker_confs, CONF_DELIMETER);
		config.cpus = atoi(ptr);
		if (!config.cpus)
				config.cpus = DEFAULT_CPUS;

		ptr = strtok(NULL, CONF_DELIMETER);
		config.memory = atoi(ptr);
		if (!config.memory)
				config.cpus = DEFAULT_MEMORY;

		return config;
}

struct testcase *__set_testcases(char *testcases)
{
		struct testcase *head = NULL;
		struct testcase *tail;
		struct testcase *tmp;
		char *_case;
		char *_ans;
		char *copycases = NULL;

		copycases = malloc(sizeof(testcases));
		if (copycases == NULL) {
				return NULL;
		}
		strcpy(copycases, testcases);

		_case = strtok(copycases, CONF_DELIMETER);
		while (_case != NULL) {
				_ans = strtok(NULL, CONF_DELIMETER);

				if (_ans == NULL) {
						while (tail != NULL) {
								tail = head->next;
								free(head);
								head = tail;
						}
						return NULL;
				}

				tmp = (struct testcase *)malloc(sizeof(struct testcase));
				assert(tmp != NULL);
				tmp->_case = _case;
				tmp->_ans = _ans;
				tmp->next = NULL;

				if (head == NULL) {
						head = tmp;
						head->next = NULL;
						tail = head;
				} else {
						tail->next = tmp;
						tail = tmp;
				}

				_case = strtok(NULL, CONF_DELIMETER);
		}

		return head;
}

int init_config(int argc, char *argv[], struct conf *config)
{
		int i;

		if (argc != 5) {
				fprintf(stderr, "argc: %d\n", argc);
				return -EINVAL;
		}

		config->lang = __select_lang(argv[1]);
		if (config->lang == -ENODATA) {
				fprintf(stderr, "Do not support lang: %s\n", argv[1]);
				return -EINVAL;
		}
		config->file_path = argv[2];
		config->docker_conf = __set_docker_conf(argv[3]);
		config->testcases = __set_testcases(argv[4]);
		if (config->testcases == NULL) {
				fprintf(stderr, "Test cases error: %s\n", argv[4]);
				return -EINVAL;
		}

		return 0;
}

int exec_cmd(char *args[], char *_stdin, char *_stdout[])
{
		pid_t pid;
		int status;
		int inp_fd, out_fd;
		char tmp[255];
		int fd1[2];
		int fd2[2];

		if (_stdin != NULL) {
				if (pipe(fd1) < 0 || pipe(fd2) < 0) {
						fprintf(stderr, "Pipe error %s\n", args[0]);
						return -EFAULT;
				}
		}

		pid = fork();
		if (pid < 0) {
				/* Error */
		} else if (pid == 0) {
				/* Child */

				if (_stdin != NULL) {
						close(fd1[1]);
						close(fd2[0]);
						dup2(fd1[0], STDIN_FILENO);
						dup2(fd2[1], STDOUT_FILENO);
				} else {
						close(STDOUT_FILENO);
				}

				/* fd automatically close when process exited */
				execv(args[0], args);
				fprintf(stderr, "Failed to exec %s\n", args[0]);
				exit(EXIT_FAILURE);
		} else {
				/* Parent */
				if (_stdin != NULL) {
						close(fd1[0]);
						write(fd1[1], _stdin, strlen(_stdin)+1);
						close(fd1[1]);
				}
				if (wait(&status) < 0) {
						fprintf(stderr, "Cannot wait pid %d\n", pid);
						return -EFAULT;
				}
				if (_stdin != NULL) {
						char buffer[65536];
						close(fd2[1]);
						memset(buffer, 0, sizeof(buffer));
						read(fd2[0], buffer, sizeof(buffer));
						strcpy(*_stdout, buffer);
				}

				if (status) {
						return status;
				}
		}
}


