import subprocess
import time

# ./hackbench -s 10000 -g 1024 -i
# tends to be a reasonable background workload

SERVER_ADDRESS = "10.34.167.145"

n_requests = 10

times = 100

print(f"Running for a total of {times}")

for i in range(times):
    print(f"iteration {i}")
    subprocess.run(["./http_client", "-i", SERVER_ADDRESS, "-n", str(n_requests)])
    time.sleep(10)