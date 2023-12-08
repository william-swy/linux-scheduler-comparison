## Swapping Kernels
List all available kernels
```sh
sudo grub-mkconfig | grep -iE "menuentry 'Ubuntu, with Linux" | awk '{print i++ " : " $1, $2, $3, $4, $5, $6, $7}'
```

Edit `/etc/default/grub` and replace the value of `GRUB_DEFAULT` with `1><index of kernel to use>`

Run `sudo update-grub`