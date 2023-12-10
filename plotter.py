import matplotlib.pyplot as plt

def extract_time(line):
    line = line.rstrip()
    line = line.split("[TEST_PROG_LOG] Time: ")[1]
    line = line.split(" ms")[0]
    return float(line)

def output_file_to_data(file_name):
    with open(file_name) as file:
        lines = [extract_time(line) for line in file]
    return lines

test_case = "ffmpeg"
platform = "server"


cfs = output_file_to_data(f"{test_case}/{platform}/cfs_multicore")
eevdf = output_file_to_data(f"{test_case}/{platform}/eevdf_multicore")
cfs_na = output_file_to_data(f"{test_case}/{platform}/cfs_multicore_no_autogroup")
eevdf_na = output_file_to_data(f"{test_case}/{platform}/eevdf_multicore_no_autogroup")

plt.hist(cfs, alpha=0.5, bins=50, label="cfs")
plt.hist(eevdf, alpha=0.5, bins=50, label='eevdf')
# plt.hist(cfs_na, alpha=0.5, bins=50, label="cfs no autogroup")
# plt.hist(eevdf_na, alpha=0.5, bins=50, label="eevdf no autogroup")
plt.legend(loc="upper right")
plt.show()