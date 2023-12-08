This test attempts to decompress gcc release source

To get the "tar.gz" test input, run
```sh
wget https://gcc.gnu.org/pub/gcc/releases/gcc-13.2.0/gcc-13.2.0.tar.gz
```

# On RPI
nohup ./launcher -g 8 -r 80 -H <path to hackbench> <output_file_name> &