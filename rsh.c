#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff","cd","exit","help"};

// return index if cmd is one of the allowed commands
// return -1 otherwise
int isAllowed(const char*cmd) {
	for (int i = 0; i < N; i++) {
		if (!strcmp(cmd, allowed[i])) {
			return i;
		}
	}
	return -1;
}

int main() {
    char line[256];
	char* argv[20];
	int argc = 0;

    while (1) {

		fprintf(stderr,"rsh>");

		if (fgets(line,256,stdin)==NULL) continue;

		if (strcmp(line,"\n")==0) continue;

		line[strlen(line)-1]='\0';

		int i = 0;
		char* arg = strtok(line, " ");
		while (arg != NULL) {
			argv[i++] = arg;
			arg = strtok(NULL, " ");
		}
		argv[i] = NULL;
		argc = i;

		// Check if command is allowed
		i = isAllowed(argv[0]);
		if (i == -1) {
			printf("NOT ALLOWED!\n");
			continue;
		}

		// First 9 commands
		if (i <= 8) {
			pid_t pid;
			int status;
			posix_spawnattr_t attr;

			// Initialize spawn attributes
			posix_spawnattr_init(&attr);

			// Spawn a new process
			if (posix_spawnp(&pid, argv[0], NULL, &attr, argv, environ) != 0) {
				perror("spawn failed");
				exit(EXIT_FAILURE);
			}

    		// Wait for the spawned process to terminate
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid failed");
				exit(EXIT_FAILURE);
			}

			// Destroy spawn attributes
    		posix_spawnattr_destroy(&attr);
		}

		// Last 3 commands
		switch (i) {
		case 9: // cd
			// More than one argument
			if (argc > 2) {
				printf("-rsh: cd: too many arguments\n");
				break;
			}
			// Change directory to the specified one
			chdir(argv[1]);
			break;
		case 10: // exit
			return 0;
			break;
		case 11: // help
			printf("The allowed commands are:\n");
			printf("1: cp\n");
			printf("2: touch\n");
			printf("3: mkdir\n");
			printf("4: ls\n");
			printf("5: pwd\n");
			printf("6: cat\n");
			printf("7: grep\n");
			printf("8: chmod\n");
			printf("9: diff\n");
			printf("10: cd\n");
			printf("11: exit\n");
			printf("12: help\n");			
			break;
		default: // unreachable
			break;
		}
    }
    return 0;
}
