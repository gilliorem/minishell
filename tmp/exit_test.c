#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Global last exit status in the parent (just like g_exit_status)
int g_status = 0;

int main(void)
{
    pid_t pid;
    int status;

    printf("Parent PID = %d\n\n", getpid());

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    // ------------------------------------------------------------
    // CHILD PROCESS
    // ------------------------------------------------------------
    if (pid == 0) {
        printf("[child] PID = %d\n", getpid());
        printf("[child] exiting with code 127\n\n");

        // Child exits with 127. Parent survives.
        exit(127);
    }

    // ------------------------------------------------------------
    // PARENT PROCESS
    // ------------------------------------------------------------
    printf("[parent] Waiting for child %d\n", pid);

    waitpid(pid, &status, 0);

    printf("[parent] Raw wait() status = %d\n", status);

    // Decode the raw wait status correctly
    if (WIFEXITED(status)) {
        g_status = WEXITSTATUS(status);
        printf("[parent] Child exited normally with exit code %d\n", g_status);
    }
    else if (WIFSIGNALED(status)) {
        g_status = 128 + WTERMSIG(status);
        printf("[parent] Child was killed by signal %d (exit code %d)\n",
               WTERMSIG(status), g_status);
    }

    printf("\n[parent] Global status now = %d\n", g_status);

    printf("[parent] Now exiting PROGRAM with that same code.\n");

    exit(g_status);
}

