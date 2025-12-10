#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int g_status = 0;

void run_command(char *input)
{
    char *argv[64];
    int argc = 0;

    char *tok = strtok(input, " \t\n");
    while (tok && argc < 63) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;

    if (argc == 0)
        return;

    /* ------------------ EXIT BUILTIN ------------------ */
    if (strcmp(argv[0], "exit") == 0) {

        if (argc == 1) {
            printf("exit\n");
            exit(g_status);
        }

        char *arg = argv[1];
        for (int i = 0; arg[i]; i++) {
            if (arg[i] < '0' || arg[i] > '9') {
                printf("exit\n");
                printf("numeric argument required\n");
                exit(2);
            }
        }

	// does not update the exit status
        if (argc > 2) {
            printf("exit\n");
            printf("too many arguments\n");
            return; /* shell continues */
        }

        /* one argument */

        long val = strtol(arg, NULL, 10);
        unsigned char code = (unsigned char)val;

        printf("exit\n");
        exit(code);
    }

    /* ------------------ ECHO $? ------------------ */
    if (strcmp(argv[0], "echo") == 0 && argc == 2 && strcmp(argv[1], "$?") == 0) {
        printf("%d\n", g_status);
        return;
    }

    /* ----- GDB SWITCH TO CHILD PROCESS --------- */
    /* set detach on-fork off
     * info inferiors
     * inferior <number> 
     * youhouu */
    /* ------------------ RUN EXTERNAL CMD ------------------ */
    pid_t pid = fork();

    if (pid == 0) {
        /* Child process */
        execvp(argv[0], argv);
        perror("execvp");
        exit(127);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status))
    {
	printf("child has finish his job normally. back to parent process.\n");
        g_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
	printf("signal trigger while runing child\n");
        g_status = 128 + WTERMSIG(status);
    }
}

/* ------------------ SIGINT HANDLER ------------------ */
void sigint_handler(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    write(1, "mini> ", 6);
    g_status = 130;
}
void sigquit_handler(int sig)
{
	(void)sig;
	printf("quit (Core dump)");
	g_status = 131;
}

int main(void)
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);

    char line[256];

    while (1) {
        printf("mini> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("exit\n");
            exit(g_status);
        }

        run_command(line);
    }
}

