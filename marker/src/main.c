#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conf.h>
#include <lang.h>
#include <testcase.h>
#include <limits.h>

void prepare_lang(struct conf *conf);
void to_json(struct score score, char *results);

/*
 * ./marker [lang] [filepath] [docker_conf] [testcases] [compile_limit] [exec_limit]
 */
int main(int argc, char *argv[])
{
		struct conf conf;
		struct score score;
		char results[4096];
		char temp[PATH_MAX * 2];
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
		 * Initialize docker
		 * (create docker container)
		 */
		if (ret = conf.langsw[conf.lang].create(&conf)) {
				fprintf(stderr, "docker creation error!\n");
				goto out;
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
		sprintf(temp, "docker rm -f %s", conf.docker_conf.name);
		system(temp);
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
