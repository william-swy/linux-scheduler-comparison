## Building For RPi
Follow instructions [from RPi website](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel)

Inside source directory, run
```sh
KERNEL=kernel8
make bcm2711_defconfig
```

Build via
```sh
make -j4 Image.gz modules dtbs
```

Install modules
```sh
sudo make modules_install
```

Copy over DTB
```sh
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /boot/overlays/
```
The documentation says it should be placed in the `/boot/firmware`
directory but for the Bullseye version, this folder does not exist.
Also the old versions of the DTB does not work as not copying over the 
new versions results in WIFI not functioning. Interestingly, swapping between
version `6.5` and `6.6` branches of the kernel does not any issues if the dtbs
are not replaced. It is only when upgrading from the `6.1` (the version that Bullseye uses) to `6.5` that has this issue.

Copy over kernel
```sh
sudo cp arch/arm64/boot/Image.gz /boot/$KERNEL-<version>.img
```
We chose to give the kernel a slightly different name than the original file
as it gives the flexibility of reverting to old version if the new version fails
to boot. Version can be `cfs` or `eevdf` or just anything to make it easy
to distinguish between the two.

Specify which kernel to boot by adding `kernel=kernel8-<version>.img` to the
end of `config.txt`

Once booted, the kernel version can be verified by running `uname -mrs`

## Building For Server