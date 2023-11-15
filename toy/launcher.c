#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/*
args:
min hackbench pairs: number at least 1
max hackbench pairs: number at least `min hackbench pairs`
hackbench pairs step size: number at least 1

min fib number: number at least 1
max fib number: number at least `min fib number`
fib number step size: number at least 1

number of repetitions to run a parameter: number at least 1

pin to core zero: 1 for yes | 0 for no

*/
static int min_hackbench_pairs = 1;
static int max_hackbench_pairs = 1;
static int hackbench_pairs_step_size = 1;

static int min_fib_number = 1;
static int max_fib_number = 1;
static int fib_number_step_size = 1;

static int num_reps = 1;

static int pin_to_core = 0;


void run_trial(const char* pairs, const char* fib_num, int pin_to_core) {
    pid_t hackbench = fork();
    if (hackbench == 0) {
        if (pin_to_core == 1) {
            execl("hackbench", "hackbench", "-s", "100000", "-g", pairs, "-i", "-a", (char*)NULL);
        } else {
            execl("hackbench", "hackbench", "-s", "100000", "-g", pairs, "-i", (char*)NULL);
        }
        
    }

    // let hackbench run for a bit to ensure all processes have been spawned
    printf("Letting hackbench run for a bit to ensure all processes have been spawned\n");
    sleep(10);

    pid_t fib = fork();
    if (fib == 0) {
        if (pin_to_core) {
            execl("fib", "fib", fib_num, "0", (char*)NULL);
        } else {
            execl("fib", "fib", fib_num, "-1", (char*)NULL);
        }
        
    }

    pid_t wait_pid = wait(NULL);
    if (wait_pid != fib) {
        printf("hackbench pid: %d\n", hackbench);
        printf("fib pid: %d\n", fib);
        printf("ERR: got %d instead of expected fib\n", wait_pid);
        return;
    } 

    kill(hackbench, SIGTERM);

    wait_pid = wait(NULL);
    if (wait_pid != hackbench) {
        printf("ERR: got %d instead of expected hackbench\n", wait_pid);
        return;
    }
}



int main(int argc, char** argv) {
    if (argc != 9) {
        printf("Incorrect arguments. Check comments\n");
        return -1;
    }

    min_hackbench_pairs = atoi(argv[1]);
    max_hackbench_pairs = atoi(argv[2]);
    hackbench_pairs_step_size = atoi(argv[3]);

    min_fib_number = atoi(argv[4]);
    max_fib_number = atoi(argv[5]);
    fib_number_step_size = atoi(argv[6]);

    num_reps = atoi(argv[7]);

    pin_to_core = atoi(argv[8]);

    for (int num_pairs = min_hackbench_pairs; num_pairs <= max_hackbench_pairs; num_pairs += hackbench_pairs_step_size) {
        int pair_buff_size = snprintf(NULL, 0,"%d",num_pairs);
        char* pair_buff = malloc(pair_buff_size + 1);
        sprintf(pair_buff, "%d", num_pairs);


        for (int fib_num = min_fib_number; fib_num <= max_fib_number; fib_num += fib_number_step_size) {
            int fib_num_buff_size = snprintf(NULL, 0,"%d",fib_num);
            char* fib_num_buff = malloc(fib_num_buff_size + 1);
            sprintf(fib_num_buff, "%d", fib_num);

            for (int rep = 0; rep < num_reps; rep++) {
                printf("[LAUNCHER LOG] pair: %d fib: %d pin: %d rep: %d\n", num_pairs, fib_num, pin_to_core, rep);

                run_trial(pair_buff, fib_num_buff, pin_to_core);
                
            }

            free(fib_num_buff);
        }

        free(pair_buff);
    }
    
}