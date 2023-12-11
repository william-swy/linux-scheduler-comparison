#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sched.h>

#include <chrono>
#include <iostream>
#include <cstring>
#include <thread>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>
#include <cstdlib>

#define HACKBENCH_DATA_SIZE "100000"

constexpr auto file_to_remove{"gcc-13.2.0.tar"};

using namespace std::chrono_literals;


// expects images folder to already exist
void run_trial(const char* hackbench_abs_path, const char* pairs, int pin_to_core, std::fstream& out, bool use_out) {
    const auto path{std::filesystem::current_path()/file_to_remove};
    if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
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
        if (execl("/usr/bin/gzip", "/usr/bin/gzip", "-k", "-d", "gcc-13.2.0.tar.gz", (char*)NULL) < 0) {
            perror("execl failed");
            std::exit(-1);
        }
    }

    std::this_thread::sleep_for(1s);

    pid_t hackbench = fork();
    if (hackbench == 0) {
        int ret{0};
        if (pin_to_core != -1) {
            std::string core{std::to_string(pin_to_core)};
            ret = execl(hackbench_abs_path, "hackbench", "-s", HACKBENCH_DATA_SIZE, "-g", pairs, "-i", "-p", core.c_str(), (char*)NULL);
        } else {
            ret = execl(hackbench_abs_path, "hackbench", "-s", HACKBENCH_DATA_SIZE, "-g", pairs, "-i", (char*)NULL);
        }
        if (ret == -1) {
            perror("Failed to spawn hackbench");
            std::exit(-1);
        }
    }

    pid_t wait_pid = wait(NULL);
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double, std::milli> ms_double{end - start};

    if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
    }

    if (wait_pid != test_proc_pid) {
        std::cerr << "ERROR waited for " << test_proc_pid << " got " << wait_pid << " hackbench pid " << hackbench << std::endl; 
        return;
    }    

    if(use_out) {
        out << "[TEST_PROG_LOG] Time: " << ms_double.count() << " ms" << std::endl;
    }
    std::cout << "[TEST_PROG_LOG] Time: " << ms_double.count() << " ms" << std::endl;

    kill(hackbench, SIGTERM);

    const pid_t hackbench_wait_pid = wait(NULL);
    if (hackbench_wait_pid != hackbench) {
        std::cerr << "ERROR got " << hackbench_wait_pid << " instead of expected hackbench" << std::endl;
        return;
    }
}

namespace {
    std::vector<std::string> string_to_vec_by_delim(const std::string& s, const std::string& delimiter) {
        std::size_t last{0}; 
        std::size_t next{0}; 

        std::vector<std::string> res{};
        
        while ((next = s.find(delimiter, last)) != std::string::npos) {   
            res.push_back(s.substr(last, next-last)); 
            last = next + 1; 
        } 
        res.push_back(s.substr(last));

        return res;
    }

    void print_usage_and_exit() {
        std::cout << "-g --groups [number of groups | (number of groups lower bound, number of groups upper bound, number of groups step size)] \t number of hackbench sender receiver pairs to spawn\n";
        std::cout << "-p --pin [core number to pin to]\t Omit arg to not pin to core\n";
        std::cout << "-r --reps [number of repetitions to run] \t Has to be greater than 0. \n";
        std::cout << "-f --file [path] \t file to output measurement results to. Omit arg to not output measurements to file\n";
        std::cout << "-H --hack [path] \t absolute path to hackbench executable\n";
        std::cout << "-h --help \t show usage\n";
        std::exit(-1);
    }

    struct Args {
        int groups_lower_bound{1};
        int groups_upper_bound{1};
        int groups_step_size{1};
        int core_to_pin_to{-1};
        int reps{1};
        std::string file_name{};
        std::fstream file{};
        bool file_valid{false};
        std::string hackbench_abs_path{"hackbench"};
    };

    std::ostream& operator<< (std::ostream& stream, const Args& args) {
        stream << "hackbench executable path: " << args.hackbench_abs_path
        << " group_lower_bound: " << args.groups_lower_bound 
        << " groups_upper_bound: " << args.groups_upper_bound 
        << " groups_step_size: " << args.groups_step_size 
        << "core_to_pin_to: " << args.core_to_pin_to 
        << " reps: " << args.reps;
        if (args.file_valid) {
            stream << " to file: " << args.file_name;
        } else {
            stream << " not to file";
        }
        return stream;
    }

    Args parse_args(int argc, char** argv) {
        auto error{false};

        Args args{};

        int optind{0};

        while(true) {
            static struct option longopts[] = {
                {"groups",  required_argument, NULL, 'g'},
                {"pin",     required_argument,	 NULL, 'p'},
                {"reps",     required_argument, NULL, 'r'},
                {"file",     required_argument, NULL, 'f'},
                {"hack",     required_argument, NULL, 'H'},
                {"help",      no_argument,	 NULL, 'h'},
                {NULL, 0, NULL, 0}
            };

            int c = getopt_long(argc, argv, "g:p:r:f:H:h", longopts, &optind);

            if (c == -1) {
                break;
		    }
            // Input validation a bit lazy here
            switch(c) {
                case static_cast<int>('g'): {
                    if (!argv[optind]) {
                        std::cerr << "Missing arg for -g\n";
                        error = true;
                        break;
                    }
                    std::string groups{optarg};
                    if (groups[0] == '(') {
                        groups = groups.substr(1, groups.size() - 2);
                        const auto tup{string_to_vec_by_delim(groups, ",")};
                        args.groups_lower_bound = std::stoi(tup[0]);
                        args.groups_upper_bound = std::stoi(tup[1]);
                        args.groups_step_size = std::stoi(tup[2]);
                    } else {
                        int num_group{std::stoi(groups)};
                        args.groups_lower_bound = num_group;
                        args.groups_upper_bound = num_group;
                    }
                    break;
                }
                case static_cast<int>('p'): {
                    if (!argv[optind]) {
                        std::cerr << "Missing arg for -p\n";
                        error = true;
                        break;
                    }
                    std::string proc{optarg};
                    args.core_to_pin_to = std::stoi(proc);
                    break;
                }
                case static_cast<int>('r'): {
                    if (!argv[optind]) {
                        std::cerr << "Missing arg for -r\n";
                        error = true;
                        break;
                    }
                    std::string reps{optarg};
                    args.reps = std::stoi(reps);
                    break;
                }
                case static_cast<int>('f'): {
                    if (!argv[optind]) {
                        std::cerr << "Missing arg for -f\n";
                        error = true;
                        break;
                    }
                    std::string file{optarg};
                    args.file_name = file;
                    args.file = std::fstream(file, std::ios::out | std::ios::in | std::ios::trunc);
                    args.file << std::fixed << std::setprecision(12);
                    args.file_valid = true;
                    break;
                }
                case static_cast<int>('H'): {
                    if (!argv[optind]) {
                        std::cerr << "Missing arg for -H\n";
                        error = true;
                        break;
                    }
                    std::string path{optarg};
                    args.hackbench_abs_path = path;
                    break;
                }
                case static_cast<int>('h'): {
                    print_usage_and_exit();
                    break;
                }
                default: {
                    error=true;
                }
                    
            }
        }
        if (error) {
            print_usage_and_exit();
        }
        return args;
    }
}



int main(int argc, char** argv) {
    auto args = parse_args(argc, argv);

    std::cout << "Running with " << args << std::endl;

    for (int num_pairs = args.groups_lower_bound; num_pairs <= args.groups_upper_bound; num_pairs+=args.groups_step_size) {
        const auto num_pairs_str{std::to_string(num_pairs)};

        for (int rep = 0; rep < args.reps; rep++) {
            std::cout << "pair count: " << num_pairs << " pin: " << args.core_to_pin_to << " rep: " << rep <<  std::endl;
            run_trial(args.hackbench_abs_path.c_str(), num_pairs_str.c_str(), args.core_to_pin_to, args.file, args.file_valid);
        }
    }    
}