import matplotlib.pyplot as plt
import numpy as np

machine = "rpi"
scheduler = "cfs_multicore"

# Returns times in nanoseconds
def read_scheduler(path):
    with open(path) as file:
        times = [float(line.rstrip().split("Total nanosec: ")[1]) for line in file]
    return times

def read_ping(path):
    with open(path) as file:
        times = [float(line.rstrip().split("time=")[1].split(" ms")[0])*1000000 for line in file]
    return times


filename = "cfs_multicore.tsv"

http_times = read_scheduler(f"{machine}/{scheduler}")
ping_times = read_ping(f"{machine}/ping_{scheduler}")

print(http_times[1])
print(ping_times[1])

# in nanosec
delta = 10000 # aka 0.01 ms