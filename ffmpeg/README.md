## Running

## Running in background
Use
```sh
nohup ./launcher 8 8 10 80 0 <output_file_name> &
```
This will run the test 80 times where the results are placed in `<output_file_name>`

For each test, 2*8 = 16 background hackbench processes are created and ffmpeg attempts to decode the
specified video. None of the processes are pinned to a processor.