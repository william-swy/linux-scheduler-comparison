import matplotlib.pyplot as plt

platform = "server"

def parse_file(path):
    with open(path) as file:
        times = [float(line.rstrip().split("Total nanosec: ")[1]) for line in file]
    return times

cfs = parse_file(f"{platform}/cfs_multicore")
eevdf = parse_file(f"{platform}/eevdf_multicore")


plt.hist(cfs, bins=50, alpha=0.5, label="cfs")
plt.hist(eevdf, bins=50, alpha=0.5, label="eevdf")
plt.legend(loc="best")
plt.show()