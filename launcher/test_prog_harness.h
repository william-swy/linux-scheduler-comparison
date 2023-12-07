#pragma once

#include "unistd.h"

#include <chrono>
#include <tuple>

// (spawned pid, waited pid, time took)
std::tuple<pid_t, pid_t, std::chrono::duration<double, std::milli>>  run_prog(int pin_to_core);