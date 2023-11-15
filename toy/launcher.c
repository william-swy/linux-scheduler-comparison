#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


int main() {
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


    pid_t hackbench = fork();
    if (hackbench == 0) {
        // dup2(child_write[1], STDOUT_FILENO);
        execl("hackbench", "hackbench", "-s", "100000", "-i", "-a", (char*)NULL);
    }

    pid_t fib = fork();
    if (fib == 0) {
        execl("fib", "fib", "45", "0", (char*)NULL);
    }

    pid_t wait_pid = wait(NULL);
    if (wait_pid != fib) {
        printf("hackbench pid: %d\n", hackbench);
        printf("fib pid: %d\n", fib);
        printf("ERR: got %d instead of expected fib\n", wait_pid);
        return -1;
    } 

    kill(hackbench, SIGTERM);

    wait_pid = wait(NULL);
    if (wait_pid != hackbench) {
        printf("ERR: got %d instead of expected hackbench\n", wait_pid);
    }


    printf("Done\n");
}