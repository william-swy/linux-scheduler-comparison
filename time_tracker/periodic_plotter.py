import matplotlib.pyplot as plt
import numpy as np

def parse_snapshot(content, j):
    
    lines = content.split('\n')

    utime = float(lines[0].split(" ")[0])

    hackbench_times = []

    gzip = None

    for i, line in enumerate(lines):
        if i == 0:
            continue
        if "hackbench" in line:
            res = lines[i+1].split(":")
            if len(res) < 2:
                continue
                # print(j)
                # print(line)
                # print(lines[i+1])
            exec_start = float(lines[i+1].split(":")[1])
            vruntime  = float(lines[i+2].split(":")[1])
            sum_exec = float(lines[i+3].split(":")[1])/1000
            hackbench_times.append((exec_start, vruntime, sum_exec))
        elif "toy_periodic" == line[:4]:
            exec_start = float(lines[i+1].split(":")[1])
            vruntime  = float(lines[i+2].split(":")[1])
            sum_exec = float(lines[i+3].split(":")[1])/1000
            gzip = (exec_start, vruntime, sum_exec)


    return (utime, hackbench_times, gzip)


res = open('eevdf_periodic_tracker', 'r').read()

time_pts = res.split("[TIME] ")[1:]


processed = []

for i, pt in enumerate(time_pts):
    if pt.count("hackbench") == 5:
        processed.append(parse_snapshot(pt, i))

times = []
for i in processed:
    times.append(i[0])

min_time = min(times)


utime_list = []
gzip_utime_list = []
gzip_sum_exec = []
gzip_vruntime = []

hackbench_sum_exec_list = [[], [], [], [], []]
hackbench_vruntime_list = [[], [], [], [], []]

for utime, hackbench_times, gzip in processed:
    utime_list.append(utime - min_time)
    if gzip is not None:
        gzip_utime_list.append(utime - min_time)
        gzip_sum_exec.append(gzip[2])
        gzip_vruntime.append(gzip[1])
       
    for i, hb in enumerate(hackbench_times):
        (exec_start, vruntime, sum_exec) = hb
        hackbench_sum_exec_list[i].append(sum_exec)
        hackbench_vruntime_list[i].append(vruntime)


plt.plot(gzip_utime_list, gzip_vruntime, label="sleeper")

for i, h in enumerate(hackbench_vruntime_list):
    if i == 0:
        continue
    plt.plot(utime_list, h, label=f"hackbench {i}")


# plt.plot(gzip_utime_list, gzip_sum_exec, label="sleeper")

# for i, h in enumerate(hackbench_sum_exec_list):
#     if i == 0:
#         continue
#     plt.plot(utime_list, h, label=f"hackbench {i}")



# ideal_usage = 0.20*np.array(gzip_utime_list)-2

# plt.plot(gzip_utime_list, ideal_usage, label="sleeper ideal fair usage")

# def ideal(x):
#     if x <= 10:
#         return 0.25*x
#     else:
#         return 0.2*x + 0.5

# hackbench_ideal_usage = [ideal(x) for x in utime_list]
# plt.plot(utime_list, hackbench_ideal_usage, label="hackbench ideal fair usage")

plt.xlabel("Time (s)")
plt.ylabel("vruntime time")
plt.title("sleeper With EEVDF")

plt.legend(loc="best")    
plt.show()

# def parse(content, j):
#     uptime = float(content.split(" ")[0])

#     hackbench = [{}, {}, {}, {}, {}]
#     gzip = {}

#     lines = content.split('\n')

#     hackbench_count = 0

#     for i, lin in enumerate(lines):
#         if "hackbench" in lin:
#             if len(lines[i+1].split(":")) < 2:
#                 print(j)
#                 print(i)
#                 print(lines[i+1])
#             exec_start = float(lines[i+1].split(":")[1])
#             vruntime  = float(lines[i+2].split(":")[1])
#             sum_exec = float(lines[i+3].split(":")[1])/1000
#             hackbench[hackbench_count]["exec_start"] = exec_start
#             hackbench[hackbench_count]["vruntime"] = vruntime
#             hackbench[hackbench_count]["sum_exec"] = sum_exec
#             hackbench_count += 1

#     return uptime, hackbench



# res = open('gzip_original_cfs', 'r').read()
# snapshots = res.split("[TIME] ")[1:]

# parsed = [parse(line, j) for j, line in enumerate(snapshots)]

# print(len(parsed))