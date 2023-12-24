import matplotlib.pyplot as plt
import numpy as np

platform = "Server"

def parse_file(path):
    with open(path) as file:
        times = [float(line.rstrip().split("Total nanosec: ")[1])/1e6 for line in file]
    return times

cfs = parse_file(f"{platform}/cfs_multicore")
eevdf = parse_file(f"{platform}/eevdf_multicore")


plt.hist(cfs, bins=50, alpha=0.5, label="cfs")
plt.hist(eevdf, bins=50, alpha=0.5, label="eevdf")
plt.xlabel('RTT time for 10 requests (ms)')
plt.title("Apache request time frequency, Server machine, with autogroup")
plt.legend(loc="best")
plt.show()

print(np.mean(cfs))
print(np.sqrt(np.var(cfs)))
print(np.mean(eevdf))
print(np.sqrt(np.var(eevdf)))