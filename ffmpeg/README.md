Requires installation of ffmpeg

Requires a mp4 named `this_is_the_way.mp4` to be placed in the same directory. This can be any mp4 renamed to this name.

# On RPI
nohup ./test_ffmpeg -g 8 -r 80 -H <path to hackbench> -f <output_file_name> &

# On server
nohup ./test_ffmpeg -g 128 -r 160 -H <path to hackbench> -f <output_file_name> &