Assumes Apache is disable by default, i.e. `systemctl disable apache2` has been run

`ab` is used as the utility to invoke requests. Can be installed via `apt-get install -y apache2-utils`

### For RPi Server
```sh
cd ../toy
./hackbench -s 100000 -g 50 -i
```
This will spawn `50*40 = 2000` background processes. None of them are pinned to a specific core


### On Client side
```sh
ab -n 10000 -c 10 -g <output_file> http://128.189.237.146/
```
This will execute `10000` requests by allowing `100` to execute in parallel at a time

Definitions of measured values can be found here: https://www.datadoghq.com/blog/apachebench/