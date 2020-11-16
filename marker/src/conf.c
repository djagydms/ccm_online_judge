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
void __set_filename(char *filename, char *filepath);
int __set_docker_conf(struct docker_conf *conf, char *delimeter, char *docker_confs);
struct testcase *__set_testcases(char *testcases, char *delimeter);
long long int __set_compile_limit(char *time);
long long int __set_exec_limit(char *time);

int init_config(int argc, char *argv[], struct conf *conf)
{
		int ret = 0;

		if (argc != 8) {
				fprintf(stderr, "argc: %d\n"
								"Usage: marker [lang] [filepath] [delimeter] [docker_conf] "
								"[testcases] [compile_limit] [exec_limit]\n", argc);
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
		__set_filename(conf->filename, conf->filepath);
		conf->delimeter = argv[3];
		/* __set_docker_conf() do not causes error */
		ret = __set_docker_conf(&conf->docker_conf, conf->delimeter, argv[4]);
		if (ret) {
				fprintf(stderr, "Docker configuration error: %s\n", argv[2]);
				return -ret;
		}
		conf->testcases = __set_testcases(argv[5], conf->delimeter);
		if (conf->testcases == NULL) {
				fprintf(stderr, "Test cases error: %s\n", argv[5]);
				return -EINVAL;
		}
		conf->compile_limit = __set_compile_limit(argv[6]);
		conf->exec_limit = __set_exec_limit(argv[7]);

		return ret;
}

pid_t pid;
int timeout;

void timer_handler(int signum) {
		timeout = 1;
		kill(pid, SIGKILL);
}

int exec_cmd(char *argv[], char *_stdin, char *_stdout[], long long int limit_time)
{
		int inp_fd, out_fd;
		char tmp[255];
		int fd1[2];
		int fd2[2];
		int status;

		if (_stdin != NULL) {
				if (pipe(fd1) < 0 || pipe(fd2) < 0) {
						fprintf(stderr, "Pipe error %s\n", argv[0]);
						return -EFAULT;
				}
		}

		timeout = 0;
		signal(SIGALRM, timer_handler);

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
						write(fd1[1], _stdin, strlen(_stdin));
						close(fd1[1]);
				}
				alarm(limit_time / NSEC_TO_SEC);
				if (wait(&status) < 0) {
						fprintf(stderr, "Cannot wait pid %d\n", pid);
						return -EFAULT;
				}
				if (timeout) {
						return -ETIMEDOUT;
				}
				if (_stdout != NULL) {
						char buffer[65536];
						close(fd2[1]);
						memset(buffer, 0, sizeof(buffer));
						read(fd2[0], buffer, sizeof(buffer));
						strcpy(*_stdout, buffer);
				}
		}

		return status;
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

void __set_filename(char *filename, char *filepath)
{
		char *tmp = NULL;

		strcpy(filename, filepath);

		tmp = strtok(filename, "/");
		filename = tmp;
		while (tmp != NULL) {
				filename = tmp;
				tmp = strtok(NULL, "/");
		}
}

int __set_docker_conf(struct docker_conf *conf, char *delimeter, char *docker_confs)
{
		char *ptr;

		ptr = strtok(docker_confs, delimeter);
		if (!ptr)
				return -ENODATA;

		sprintf(conf->name, "%s", ptr);

		ptr = strtok(NULL, delimeter);
		conf->cpus = atoi(ptr);
		if (!conf->cpus)
				conf->cpus = DEFAULT_CPUS;

		ptr = strtok(NULL, delimeter);
		conf->memory = atoi(ptr) * 1024 * 1024;
		if (!conf->memory)
				conf->memory = DEFAULT_MEMORY;

		return 0;
}

struct testcase *__set_testcases(char *testcases, char *delimeter)
{
		struct testcase *head = NULL;
		struct testcase *tail;
		struct testcase *tmp;
		char *_case;
		char *_ans;
		char *copycases = NULL;

		copycases = malloc(sizeof(testcases)*10);
		if (!copycases) {
				return NULL;
		}
		strcpy(copycases, testcases);

		_case = strtok(copycases, delimeter);
		while (_case != NULL) {
				_ans = strtok(NULL, delimeter);

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
				_case = strtok(NULL, delimeter);
		}

		return head;
}

long long int __set_compile_limit(char *time)
{
		long long int limit_time = atoi(time) * SEC_TO_NSEC;
		if (!limit_time)
				limit_time = DEFAULT_COMPILE_LIMIT;

		return limit_time;
}

long long int __set_exec_limit(char *time)
{
		long long int limit_time = atoi(time) * SEC_TO_NSEC;
		if (!limit_time)
				limit_time = DEFAULT_EXEC_LIMIT;

		return limit_time;
}
