#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>

#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <fstream>
#include <ostream>



/*
args:
min hackbench pairs: number at least 1
max hackbench pairs: number at least `min hackbench pairs`
hackbench pairs step size: number at least 1

number of repetitions to run a parameter: number at least 1

pin to core zero: 1 for yes | 0 for no

*/
static int min_hackbench_pairs = 1;
static int max_hackbench_pairs = 1;
static int hackbench_pairs_step_size = 1;

static int num_reps = 1;

static int pin_to_core = 0;

// expects images folder to already exist
void run_trial(const char* pairs, int pin_to_core, std::fstream& out, bool use_out) {
    using namespace std::chrono_literals;
    std::filesystem::create_directories(std::filesystem::current_path() / "images");
    
    pid_t hackbench = fork();
    if (hackbench == 0) {
        if (pin_to_core == 1) {
            execl("hackbench", "hackbench", "-s", "100000", "-g", pairs, "-i", "-a", (char*)NULL);
        } else {
            execl("hackbench", "hackbench", "-s", "100000", "-g", pairs, "-i", (char*)NULL);
        }
    }

    // // let hackbench run for a bit to ensure all processes have been spawned
    std::printf("Letting hackbench run for a bit to ensure all processes have been spawned\n");
    std::this_thread::sleep_for(10s);

    const auto start = std::chrono::steady_clock::now();

    pid_t ffmpeg = fork();
    if (ffmpeg == 0) {
        if (pin_to_core) {
            cpu_set_t set;
            CPU_ZERO(&set);

            int core_to_pin = 0;

            CPU_SET(core_to_pin, &set);
            int res = sched_setaffinity(0, sizeof(set), &set);
            if (res != 0) {
                std::printf("Failed to pin to core %d\n", core_to_pin);
                return;
            }
        }
        if (execl("/usr/bin/ffmpeg", "/usr/bin/ffmpeg", "-v", "quiet", "-i", "this_is_the_way.mp4", "images/image-%05d.png", (char*)NULL) < 0) {
            perror("execl failed");
            exit(-1);
        }
    }

    pid_t wait_pid = wait(NULL);
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double, std::milli> ms_double = end - start;
    if(use_out) {
        out << "[FFMPEG LOG] Time: " << ms_double.count() << " ms" << std::endl;
    }
    std::cout << "[FFMPEG LOG] Time: " << ms_double.count() << " ms" << std::endl;
    

    std::filesystem::remove_all(std::filesystem::current_path() / "images");


    if (wait_pid != ffmpeg) {
        std::printf("hackbench pid: %d\n", hackbench);
        std::printf("ffmpeg pid: %d\n", ffmpeg);
        std::printf("ERR: got %d instead of expected ffmpeg\n", wait_pid);
        return;
    }

    kill(hackbench, SIGTERM);

    wait_pid = wait(NULL);
    if (wait_pid != hackbench) {
        std::printf("ERR: got %d instead of expected hackbench\n", wait_pid);
        return;
    }
}



int main(int argc, char** argv) {
    if (argc != 7) {
        std::cout << "Incorrect arguments.\n";
        std::cout << "args:\n";
        std::cout << "min hackbench pairs: number at least 1\n";
        std::cout << "max hackbench pairs: number at least `min hackbench pairs`\n";
        std::cout << "hackbench pairs step size: number at least 1\n";
        std::cout << "number of repetitions to run a parameter: number at least 1\n";
        std::cout << "pin to core zero: 1 for yes | 0 for no\n";
        std::cout << "file to place results in. Use stdout to output to stdout\n";
        return -1;
    }

    min_hackbench_pairs = std::atoi(argv[1]);
    max_hackbench_pairs = std::atoi(argv[2]);
    hackbench_pairs_step_size = std::atoi(argv[3]);

    num_reps = std::atoi(argv[4]);

    pin_to_core = std::atoi(argv[5]);

    const char* output_file = argv[6];

    std::fstream file{};
    bool use_file{false};

    if (std::strcmp(output_file, "stdout") != 0) {
        file = std::fstream(output_file, std::ios::out | std::ios::in | std::ios::trunc);
        use_file = true;
    }


    for (int num_pairs = min_hackbench_pairs; num_pairs <= max_hackbench_pairs; num_pairs += hackbench_pairs_step_size) {
        int pair_buff_size = std::snprintf(NULL, 0,"%d",num_pairs);
        const auto pair_buff = static_cast<char*>(malloc(pair_buff_size + 1));
        std::snprintf(pair_buff, pair_buff_size + 1, "%d", num_pairs);

        for (int rep = 0; rep < num_reps; rep++) {
            std::printf("[LAUNCHER LOG] pair: %d pin: %d rep: %d\n", num_pairs, pin_to_core, rep);

            run_trial(pair_buff, pin_to_core, file, use_file);
            
        }

        free(pair_buff);
    }
    
}