import matplotlib.pyplot as plt
import numpy as np

def extract_time(line):
    line = line.rstrip()
    line = line.split("ms")[0]
    return float(line)

def output_file_to_data(file_name):
    with open(file_name) as file:
        lines = [extract_time(line) for line in file]
    return lines


cfs = np.array(output_file_to_data("cfs_periodic_pinned"))
eevdf = np.array(output_file_to_data("eevdf_periodic_pinned"))

plt.hist(cfs, alpha=0.5, bins=50, label="cfs")
plt.hist(eevdf, alpha=0.5, bins=50, label='eevdf')
plt.xlabel('Actual wake up time (s)')
plt.title("Wake up time frequency, RPi machine")
plt.legend(loc="upper right")

print(np.mean(cfs))
print(np.sqrt(np.var(cfs)))
print(np.mean(eevdf))
print(np.sqrt(np.var(eevdf)))

plt.show()
