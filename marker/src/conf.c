#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <limits.h>
#include <conf.h>
#include <lang.h>
#include <testcase.h>

int __select_lang(struct langsw *langsw, char *name);
int __set_file(struct conf *conf, char *cur, char *filepath);
struct docker_conf __set_docker_conf(char *filepath, char *docker_confs);
struct testcase *__set_testcases(char *testcases);

int init_config(int argc, char *argv[], struct conf *conf)
{
		int ret = 0;

		if (argc != 5) {
				fprintf(stderr, "argc: %d\n", argc);
				return -EINVAL;
		}
		conf->lang = __select_lang(conf->langsw, argv[1]);
		if (conf->lang == -ENODATA) {
				fprintf(stderr, "Do not support lang: %s\n", argv[1]);
				return -EINVAL;
		}
		ret = __set_file(conf, argv[2], conf->filepath);
		if (ret) {
				fprintf(stderr, "File creation error: %s\n", argv[2]);
				return -ret;
		}
		/* __set_docker_conf() do not causes error */
		conf->docker_conf = __set_docker_conf(conf->filepath, argv[3]);
		conf->testcases = __set_testcases(argv[4]);
		if (conf->testcases == NULL) {
				fprintf(stderr, "Test cases error: %s\n", argv[4]);
				return -EINVAL;
		}

		return ret;
}

int exec_cmd(char *argv[], char *_stdin, char *_stdout[])
{
		pid_t pid;
		int status;
		int inp_fd, out_fd;
		char tmp[255];
		int fd1[2];
		int fd2[2];

		if (_stdin != NULL) {
				if (pipe(fd1) < 0 || pipe(fd2) < 0) {
						fprintf(stderr, "Pipe error %s\n", argv[0]);
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
				}
				if (_stdout != NULL) {
					dup2(fd2[1], STDOUT_FILENO);
				} else {
					close(STDOUT_FILENO);
				}
				/* fd automatically close when process exited */
				execv(argv[0], argv);
				fprintf(stderr, "Failed to exec %s\n", argv[0]);
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
				if (_stdout != NULL) {
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

int __select_lang(struct langsw *langsw, char *name)
{
		for (int i = 0; langsw[i].name != NULL; i++) {
				if (!strcmp(langsw[i].name, name)) {
						return i;
				}
		}

		return -ENODATA;
}

int __set_file(struct conf *conf, char *cur, char *filepath)
{
		char buffer[PATH_MAX*3];
		struct langsw *langsw = conf->langsw;
		int lang = conf->lang;
		int ret = 0;

		sprintf(buffer, "mv %s %s%s", cur, cur, langsw[lang].ext);
		if (ret = system(buffer))
				return ret;

		sprintf(filepath, "%s%s", cur, langsw[lang].ext);

		return ret;
}

struct docker_conf __set_docker_conf(char *filepath, char *docker_confs)
{
		struct docker_conf conf;
		char *ptr;

		sprintf(conf.name, "%s", filepath);

		ptr = strtok(docker_confs, CONF_DELIMETER);
		conf.cpus = atoi(ptr);
		if (!conf.cpus)
				conf.cpus = DEFAULT_CPUS;

		ptr = strtok(NULL, CONF_DELIMETER);
		conf.memory = atoi(ptr);
		if (!conf.memory)
				conf.memory = DEFAULT_MEMORY;

		return conf;
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
		if (!copycases) {
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
				if (!tmp) {
						free(copycases);
						free_testcases(head);
						return NULL;
				}
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

