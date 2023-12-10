#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sched.h>
#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>
#include <iomanip>
#include <string>
#include <fstream>

#define ADDRESS "169.254.104.216"
#define SENDER_CPU 0
#define RECEIVER_CPU 1

using namespace std::chrono_literals;

const char loop_back[] = "127.0.0.1";

namespace {
    int parent_to_child[2];
    int child_to_parent[2];
    int sock;

    const char start_msg[] = "s";
    const char ready_msg[] = "r";

    // Assume socket is already connected
    void send_get() {
        char msg [] = "GET / HTTP/1.1\r\nHost: www.scheduler_test.com\r\n\r\n";

        int remaining = strlen(msg);
        char* curr = msg;

        while(remaining > 0) {
            int ret = write(sock, curr, remaining);
            if (ret == -1) {
                perror("Write, ");
                std::exit(-1);
            }
            if (ret <= remaining) {
                remaining -= ret;
                curr += ret;
            }
        }
    }

    // Assume socket is already connected
    std::chrono::time_point<std::chrono::steady_clock> recv_until_eof() {
        char recv_buf[256];

        int total_bytes{0};

        while(true) {
            int ret = read(sock, recv_buf, 256);
            if (ret == -1) {
                perror("Read, ");
                std::exit(-1);
            }
            if (ret == 0) {
                break;
            }
            total_bytes += ret;
        }
        const auto recv_done = std::chrono::steady_clock::now();
        std::printf("%s\n", recv_buf);
        std::cout << "Read " << total_bytes << " bytes\n";
        std::cout << "recv nanosec since epoch " << std::chrono::duration_cast<std::chrono::nanoseconds>(recv_done.time_since_epoch()).count() << '\n';
        return recv_done;
    }

    void child_task() {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(RECEIVER_CPU, &cpu_set);
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

        int res{0};


        close(parent_to_child[1]);
        close(child_to_parent[0]);

        char msg_buf{0};
        res = read(parent_to_child[0], &msg_buf, 1);
        if (res != 1) {
            std::cerr << "Failed to recieve start message\n";
            std::exit(-1);
        }

        if (msg_buf != start_msg[0]) {
            std::cerr << "Recieved wrong start message\n";
        }

        
        res = write(child_to_parent[1], ready_msg, 1);
        if (res != 1) {
            std::cerr << "Failed to write ready message\n";
            std::exit(-1);
        }


        const auto recv_done = recv_until_eof();
        int64_t micro_sec_since_epoch{std::chrono::duration_cast<std::chrono::nanoseconds>(recv_done.time_since_epoch()).count()};
        int data_size{sizeof(micro_sec_since_epoch)};

        res = write(child_to_parent[1], (void*)&micro_sec_since_epoch, data_size);
        if (res != data_size) {
            std::cerr << "Failed to write end time\n";
            std::exit(-1);
        }
    }

    struct Args {
        const char* ivp4_addr = loop_back;
        int num_requests{100};
        std::string output_file{"request.log"};
    };

    void print_usage_and_exit() {
        std::cout << "-i address -f output_file -n number_requests -h help" << std::endl;
        std::exit(0);
    }

    Args parse_args(int argc, char** argv) {
        Args args{};

        int optind{0};

        static struct option longopts[] = {
            {"ip", required_argument, NULL, 'i'},
            {"num", required_argument, NULL, 'n'},
            {"file", required_argument, NULL, 'f'},
            {"help", no_argument, NULL, 'h'},
            {NULL, 0, NULL, 0}
        };

        bool error{false};

        while(true) {
            int c = getopt_long(argc, argv, "i:n:f:h", longopts, &optind);

            if (c == -1) {
                break;
            }

            switch(c) {
                case static_cast<int>('i'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -c\n";
                        error=true;
                        break;
                    }
                    args.ivp4_addr = optarg;
                    break;
                }
                case static_cast<int>('f'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -f\n";
                        error=true;
                        break;
                    }
                    args.output_file = std::string{optarg};
                    break;
                }
                case static_cast<int>('n'): {
                    if(!argv[optind]) {
                        std::cerr << "Missing arg for -t\n";
                        error=true;
                        break;
                    }
                    args.num_requests = std::stoi(optarg);
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
}



int main(int argc, char** argv) {
    const auto args = parse_args(argc, argv);

    int res{};

    res = pipe(parent_to_child);
    if (res == -1) {
        perror("parent to child,");
        std::exit(-1);
    }

    res = pipe(child_to_parent);
    if (res == -1) {
        perror("child to parent, ");
        std::exit(-1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket, ");
        std::exit(-1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    res = inet_pton(AF_INET, args.ivp4_addr, &serv_addr.sin_addr);
    if (res != 1) {
        std::cerr << "Failed to convert address\n";
        std::exit(-1);    
    }

    res = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (res != 0) {
        perror("connect, ");
        std::exit(-1);
    }
    std::cout << "Waiting for 5 sec" << std::endl;

    std::this_thread::sleep_for(5s);

    const pid_t receiver = fork();
    if (receiver == 0) {
        child_task();

        std::exit(0);
    } else if (receiver == -1) {
        perror("fork, ");
    } else {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(SENDER_CPU, &cpu_set);
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


        close(parent_to_child[0]);
        close(child_to_parent[1]);

        res = write(parent_to_child[1], start_msg, 1);
        if (res != 1) {
            std::cerr << "Failed to send start message\n";
            std::exit(-1);
        }

        char ready{0};

        res = read(child_to_parent[0], &ready, 1);
        if (res != 1) {
            std::cerr << "Failed to recieve ready message\n";
            std::exit(-1);
        }
        if (ready != ready_msg[0]) {
            std::cerr << "Recieved wrong ready message\n";
            std::exit(-1);
        }

        const int max_sends{args.num_requests};
        int total_sends{max_sends};

        const auto start{std::chrono::steady_clock::now()};

        while (total_sends > 0) {
            send_get();
            total_sends--;
        }
        

        std::cout << "Done Sending" << std::endl;

        res = shutdown(sock, 1);
        if (res == -1) {
            perror("shutdown, ");
            std::exit(-1);
        }

        int64_t end_time{0};
        int data_size{sizeof(end_time)};
        res = read(child_to_parent[0], (void*)&end_time, data_size);
        if (res != data_size) {
            std::cerr << "Failed to receive end time\n";
            std::exit(-1);
        }

        const pid_t wait_pid = wait(NULL);
        if (wait_pid != receiver) {
            std::cerr << "Error got " << wait_pid << " instead of expected receiver " << receiver << '\n';
            return -1;
        }
        int64_t ticks{std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count()};
        std::cout << "send nanosec since epoch " << ticks << '\n';
        std::cout << "Total nanosec " << end_time - ticks << '\n';
        std::ofstream out{};
        out.open(args.output_file, std::ios::app);
        out << "Total nanosec: " << end_time - ticks << std::endl;
    }    
}