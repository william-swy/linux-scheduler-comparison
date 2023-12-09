#include "test_prog_harness.h"

#include <sched.h>
#include <sys/wait.h>
#include <stdio.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <filesystem>

constexpr auto path_to_remove{"gcc-13.2.0"};

std::tuple<pid_t, pid_t, std::chrono::duration<double, std::milli>>  run_prog(int pin_to_core) {
    const auto path{std::filesystem::current_path()/path_to_remove};
    if (std::filesystem::is_directory(path)) {
        std::filesystem::remove_all(path);
    }

    const auto start = std::chrono::steady_clock::now();

    pid_t test_proc_id = fork();
    if (test_proc_id == 0) {
        if (pin_to_core != -1) {
            cpu_set_t set;
            CPU_ZERO(&set);

            CPU_SET(pin_to_core, &set);
            int res = sched_setaffinity(0, sizeof(set), &set);
            if (res != 0) {
                std::cerr << "ERROR Failed to pin to core " << pin_to_core << std::endl;
                std::exit(-1);
            }
        }
        if (execl("/usr/bin/tar", "/usr/bin/tar", "-xf", "gcc-13.2.0.tar", (char*)NULL) < 0) {
            perror("execl failed");
            std::exit(-1);
        }
    }

    pid_t wait_pid = wait(NULL);

    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double, std::milli> ms_double{end - start};

    if (std::filesystem::is_directory(path)) {
        std::filesystem::remove_all(path);
    }

    return {test_proc_id, wait_pid, ms_double};
}