This test attempts to decompress gcc release source

To get the "tar.gz" test input, run
```sh
wget https://gcc.gnu.org/pub/gcc/releases/gcc-13.2.0/gcc-13.2.0.tar.gz
```

# On RPI
nohup ./test_gzip -g 8 -r 80 -H <path to hackbench> -f <output_file_name> &

# On server
nohup ./test_gzip -g 128 -r 100 -H <path to hackbench> -f <output_file_name> &