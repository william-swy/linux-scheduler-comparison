#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    cpu_set_t set;
    CPU_ZERO(&set);

    CPU_SET(0, &set);
    int res = sched_setaffinity(0, sizeof(set), &set);
    if (res != 0) {
        printf("Failed to pin\n");
        return -1;
    }
    printf("Pinned on CPU 0\n");

    // int child_write[2]; // [0] parent reads, [1] child writes
    // int parent_write[2]; // [0] child reads, [1] parent writes
    // int child_res = pipe(child_write);
    // int parent_res = pipe(parent_write);
    // if (child_res != 0) {
    //     printf("Failed to create child pipe\n");
    //     return -1;
    // }
    // if (parent_res != 0) {
    //     printf("Failed to create parent pipe\n");
    //     return -1;
    // }


    pid_t child = fork();
    if (child == 0) {
        // dup2(child_write[1], STDOUT_FILENO);
        execl("hackbench", "hackbench", "-s", "100000", (char*)NULL);
    }

    pid_t fib = fork();
    if (fib == 0) {
        execl("fib", "fib", "45", (char*)NULL);
    }

    while(1) {
        int status;
        pid_t wait_pid = wait(&status);
        if (wait_pid == -1) {
            break;
        }
    }

    // printf("stdout of child:\n");
    // char buf[256];
    // while(1) {
    //     memset(buf, 0, 256);
    //     int count = read(child_write[0], buf, 256-1);
    //     if (count >= 0) {
    //         printf("%s", buf);
    //     } else {
    //         break;
    //     }
    // }


    printf("Done\n");
}