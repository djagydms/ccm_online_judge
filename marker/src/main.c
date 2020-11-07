#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conf.h>
#include <lang.h>
#include <testcase.h>
#include <limits.h>

void to_json(struct score score, char *results);

/*
 * ./marker [language] [filepath] [docker_conf] [testcases]
 */
int main(int argc, char *argv[])
{
		struct conf conf;
		struct score score;
		char results[4096];
		char temp[PATH_MAX * 2];
		int ret = 0;

		/*
		 * Configurations setting
		 * (language, system resources, testcase, answer)
		 */
		if (ret = init_config(argc, argv, &conf)) {
				fprintf(stderr, "confuration error!\n");
				goto out;
		}

		/*
		 * Initialize docker
		 * (create docker container)
		 */
		if (ret = langsw[conf.lang].create(&conf)) {
				fprintf(stderr, "docker creation error!\n");
				goto out;
		}

		/*
		 * Prepare docker
		 * (copy source code file, compile(if needed))
		 */
		if (ret = langsw[conf.lang].prepare(&conf)) {
				fprintf(stderr, "docker prepare error!\n");
				goto out;
		}

		/*
		 * Testing start using testcase and answer
		 */
		if (ret = langsw[conf.lang].exec(&conf, &score)) {
				fprintf(stderr, "docker exec error!\n");
				goto out;
		}

		/*
		 * Returning test results through stdout
		 */
		to_json(score, results);
		printf("%s\n", results);
out:
		free_testcases(conf.testcases);
		fclose(stdout);
		sprintf(temp, "docker rm -f %s", conf.docker_conf.name);
		system(temp);

		return ret;
}

void to_json(struct score score, char *results)
{
		sprintf(results, "{\"marking\":%s,\"exectime\":%ld}", 
						score.marking, score.exectime);
}
