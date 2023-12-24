import matplotlib.pyplot as plt
import numpy as np

def output_file_to_data(file_name):
    with open(file_name) as file:
        lines = [float(line.rstrip().split(" ")[0]) for line in file]
    return lines

cfs = output_file_to_data("rpi_cfs")
eevdf = output_file_to_data("rpi_eevdf")

plt.hist(cfs, alpha=0.5, bins=50, label="cfs")
plt.hist(eevdf, alpha=0.5, bins=50, label="eevdf")
plt.legend(loc="upper right")
plt.xlabel('RTT time for 1 query (ms)')
plt.title("Postgresql query time frequency, RPi machine, with autogroup")

plt.show()

print(np.mean(cfs))
print(np.sqrt(np.var(cfs)))
print(np.mean(eevdf))
print(np.sqrt(np.var(eevdf)))