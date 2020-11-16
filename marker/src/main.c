#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conf.h>
#include <lang.h>
#include <testcase.h>
#include <limits.h>

void prepare_lang(struct conf *conf);
void to_json(struct score score, char *results);
int update_container(struct docker_conf conf);
void clear_container(char *name, char *filepath);

/*
 * ./marker [lang] [filepath] [docker_conf] [testcases] [compile_limit] [exec_limit]
 */
int main(int argc, char *argv[])
{
		struct conf conf;
		struct score score;
		char results[4096];
		int ret = 0;

		prepare_lang(&conf);

		/*
		 * Configurations setting
		 * (language, system resources, testcase, answer)
		 */
		if (ret = init_config(argc, argv, &conf)) {
				fprintf(stderr, "confuration error!\n");
				goto conf_out;
		}

		/*
		 * Container setup and start
		 * (cpu, memory)
		 */
		if (ret = update_container(conf.docker_conf)) {
				fprintf(stderr, "container update error!\n");
				goto conf_out;
		}

		/*
		 * Prepare docker
		 * (copy source code file, compile(if needed))
		 */
		if (ret = conf.langsw[conf.lang].prepare(&conf)) {
				fprintf(stderr, "docker prepare error!\n");
				goto out;
		}

		score.status = 0;
		/*
		 * Testing start using testcase and answer
		 */
		if (ret = conf.langsw[conf.lang].exec(&conf, &score)) {
				fprintf(stderr, "docker exec error!\n");
				goto out;
		}

		/*
		 * Returning test results through stdout
		 */
		to_json(score, results);
		printf("%s\n", results);
out:
		fclose(stdout);
		clear_container(conf.docker_conf.name, conf.filename);
conf_out:
		free_testcases(conf.testcases);
		return ret;
}

void to_json(struct score score, char *results)
{
		sprintf(results, "{\"status\":%d,\"marking\":\"%s\",\"exectime\":%ld}", 
						score.status, score.marking, score.exectime);
}

void prepare_lang(struct conf *conf)
{
		for (int i=0; i<MAX_LANG; i++) {
				if (install[i] == NULL)
						break;
				conf->langsw[i] = install[i]();
		}
}

int update_container(struct docker_conf conf)
{
		char *args[6] = {DOCKER_PATH, "update",};
		char *args1[] = {DOCKER_PATH, "start", conf.name, NULL};
		char cpus[32];
		char memory[32];
		int ret;

		sprintf(cpus, "--cpus=%d", conf.cpus);
		sprintf(memory, "--memory=%d", conf.memory);

		args[2] = cpus;
		args[3] = memory;
		args[4] = conf.name;
		args[5] = NULL;

		if (ret = exec_cmd(args, NULL, NULL, 0))
				return ret;

		return exec_cmd(args1, NULL, NULL, 0);

}

void clear_container(char *name, char *filename)
{
		char temp[PATH_MAX * 2];
	
		sprintf(temp, "docker exec %s rm -f a.out", name);
		system(temp);
		sprintf(temp, "docker exec %s rm -f %s", name, filename);
		system(temp);
}
