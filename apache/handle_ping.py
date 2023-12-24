import matplotlib.pyplot as plt
import numpy as np
import math
from scipy import signal

machine = "rpi"
auto_group = True
end = ""
if not auto_group:
    end = "_no_autogroup"

# Returns times in nanoseconds
def read_scheduler(path):
    with open(path) as file:
        times = [float(line.rstrip().split("Total nanosec: ")[1]) for line in file]
    return times

def read_ping(path):
    with open(path) as file:
        times = [float(line.rstrip().split("time=")[1].split(" ms")[0])*1000000 for line in file]
    return times

def find_bucket(buckets, val, delta):
    for key in buckets:
            if (val >= key) and (val < key + delta):
                return key

def estimate_density_func(arr, z_arr, delta):
    total_elements = len(arr)
    pdf = []
    for z in z_arr:
        count = 0.0
        for item in arr:
            if (item >= z) and (item < (z + delta)):
                count += 1

        pdf.append(count / total_elements)
    return np.array(pdf)

def convolute(http_times, ping_times, delta):
    ping_times_negated = -ping_times
    min_dist_sum = int(np.min(ping_times_negated)) // delta * delta
    max_dist_sum = int(math.ceil(np.max(http_times)/delta)*delta)

    z_vals = []
    z = min_dist_sum
    while z <= max_dist_sum:
        z_vals.append(z)
        z += delta

    http_pdf = estimate_density_func(http_times, z_vals, delta)

    neg_ping_pdf = estimate_density_func(ping_times_negated, z_vals, delta)

    conv_pmf = signal.fftconvolve(http_pdf,neg_ping_pdf,'same')

    return z_vals, conv_pmf


cfs_http_times = np.array(read_scheduler(f"{machine}/cfs_multicore{end}"))
cfs_ping_times = np.array(read_ping(f"{machine}/ping_cfs_multicore{end}"))

eevdf_http_times = np.array(read_scheduler(f"{machine}/eevdf_multicore{end}"))
eevdf_ping_times = np.array(read_ping(f"{machine}/ping_eevdf_multicore{end}"))

# in nanosec
delta = 100000 # aka 0.1 ms

cfs_z, cfs_pmf = convolute(cfs_http_times, cfs_ping_times, delta)
eevdf_z, eevdf_pmf = convolute(eevdf_http_times, eevdf_ping_times, delta)

plt.plot(cfs_z, cfs_pmf, label='CFS')
plt.plot(eevdf_z,eevdf_pmf, label='EEVDF')
plt.legend(loc='best'), plt.suptitle('PDFs')
plt.xlabel("Time (ms)")
plt.ylabel("Probability")
plt.show()





