#include <filesystem>
#include <iostream>
#include <cctype>
#include <fstream>
#include <chrono>
#include <thread>

#include <getopt.h>
#include <sched.h>

using namespace std::chrono_literals;

namespace {
    void print_usage_and_exit() {
        std::cout << "-c core_to_pin_to -f output_file -t max_time_in_ms -h help" << std::endl;
        std::exit(0);
    }
    
    struct Args {
        int core_to_pin{0};
        std::string output_file{"tracker.log"};
        std::chrono::milliseconds max_duration{1000ms};
    };

    Args parse_args(int argc, char** argv) {
        Args args{};

        int optind{0};

        static struct option longopts[] = {
            {"core", required_argument, NULL, 'c'},
            {"file", required_argument, NULL, 'f'},
            {"time", required_argument, NULL, 't'},
            {"help", no_argument, NULL, 'h'},
            {NULL, 0, NULL, 0}
        };

        bool error{false};

        while(true) {
            int c = getopt_long(argc, argv, "c:f:t:h", longopts, &optind);

            if (c == -1) {
                break;
            }

            switch(c) {
                case static_cast<int>('c'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -c\n";
                        error=true;
                        break;
                    }
                    args.core_to_pin = std::stoi(optarg);
                    break;
                }
                case static_cast<int>('f'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -f\n";
                        error=true;
                        break;
                    }
                    args.output_file = optarg;
                    break;
                }
                case static_cast<int>('t'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -t\n";
                        error=true;
                        break;
                    }
                    const auto duration = std::stoi(optarg);
                    args.max_duration = std::chrono::milliseconds(duration);
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
        if(error) {
            print_usage_and_exit();
        }

        return args;
    }

    void retrieve_data(std::ofstream& output_file) {
        std::ifstream uptime_file{"/proc/uptime"};

        std::string uptime{};
        std::getline(uptime_file, uptime);
        output_file << "[TIME] " << uptime;


        const std::filesystem::path proc{"/proc"};

        for (const auto& dir_entry : std::filesystem::directory_iterator{proc}) {
            const auto path{dir_entry.path()};
            const auto filename{path.filename().c_str()};
            if (std::isdigit(filename[0])) {
                const auto sched_file{path / "sched"};

                std::ifstream sched{sched_file};
                output_file << path << "\n";
                std::string line{};
                if (!sched.is_open()) {
                    continue;
                }
                std::getline(sched, line);
                output_file << line << "\n";
                if (!sched.is_open()) {
                    continue;
                }
                std::getline(sched, line);
                if (!sched.is_open()) {
                    continue;
                }
                std::getline(sched, line);
                output_file << line << "\n";
                if (!sched.is_open()) {
                    continue;
                }
                std::getline(sched, line);
                output_file << line << "\n";
                if (!sched.is_open()) {
                    continue;
                }
                std::getline(sched, line);
                output_file << line << "\n";

                // const auto stat_path{path / "stat"};
                // std::ifstream stat{stat_path};

                // std::string line{};
                // std::getline(stat, line);
                // output_file << "," << line;
            }
        }
        output_file << '\n';
    }

}

int main(int argc, char** argv) {
    

    Args args = parse_args(argc, argv);

    std::ofstream output_file{args.output_file};

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(args.core_to_pin, &cpu_set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) < 0) {
        perror("Error (sched_setaffinity)");
        std::exit(-1);
    }

    struct sched_param param;
    param.sched_priority = 1;
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
        perror("Error (sched_setscheduler)");
        std::exit(-1);
    }

    const auto prog_start = std::chrono::steady_clock::now();

    while(true) {
        const auto cycle_start = std::chrono::steady_clock::now();
        if ((cycle_start - prog_start) >= args.max_duration) {
            break;
        }

        retrieve_data(output_file);

        const auto cycle_end = std::chrono::steady_clock::now();

        const std::chrono::duration<long double, std::micro> time_spent{cycle_end - cycle_start};

        const auto max_duration{10ms};

        if (time_spent > max_duration) {
            std::cerr << "Cycle took " << time_spent.count() << "microsec\n";
            continue;
        } else {
            std::this_thread::sleep_for(max_duration - time_spent);
        }
    }
}