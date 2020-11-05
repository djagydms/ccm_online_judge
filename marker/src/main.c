#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <conf.h>

// argv has configurations
int main(int argc, char *argv[])
{
		struct conf config;
		char *results;
		enum lang abc;

		/*
		 * Configurations setting
		 * (language, system resources, testcase, answer)
		 */
		if (init_config(argc, argv, &config)) {
				/* Error */
				fprintf(stderr, "configuration error!\n");
				exit(-EFAULT);
		}

		/*
		 * Initialize docker
		 * (create docker container)
		 */
		if (langsw[config.lang].create(&config.docker_conf)) {
				/* Error */
		}

		/*
		 * Prepare docker
		 * (copy source code file, compile(if needed))
		 */
		if (langsw[config.lang].prepare(config.file_path)) {
				/* Error */
		}

		/*
		 * Testing start using testcase and answer
		 */
		results = langsw[config.lang].exec(&config);

		/*
		 * Returning test results through stdout */
		printf("%s\n", results);

		/* TODO: free all testcases */
		free(config.testcases->_case);
		free(config.testcases);
		free(results);

		return 0;
}
