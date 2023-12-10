Assumes Apache is disable by default, i.e. `systemctl disable apache2` has been run

`ab` is used as the utility to invoke requests. Can be installed via `apt-get install -y apache2-utils`

### For RPi Server
```sh
cd ../toy
./hackbench -s 100000 -g 50 -i
```
This will spawn `50*40 = 2000` background processes. None of them are pinned to a specific core


### On Personal Device Client side
```sh
ab -n 10000 -c 10 -g <output_file> http://128.189.237.146/
```
This will execute `10000` requests by allowing `100` to execute in parallel at a time

Definitions of measured values can be found here: https://www.datadoghq.com/blog/apachebench/

Eithernet ip 169.254.104.216

### For Server Machine
```sh
cd ../toy
./hackbench -s 100000 -g 800 -i
```
This will spawn `800*40 = 32000` background processes. None of them are pinned to a specific core

### On Server Client side
```sh
ab -n 100000 -c 100 -g <output_file> http://smarties07.cs.ubc.ca/
```

Info on apache architecture 
https://medium.com/brundas-tech-notes/apache-http-server-multi-process-architecture-8fb14438a2ce

Info on apache directives
https://httpd.apache.org/docs/2.4/mod/mpm_common.html#maxrequestworkers

Configuring apache
https://www.digitalocean.com/community/tutorials/how-to-configure-the-apache-web-server-on-an-ubuntu-or-debian-vps#the-apache-file-hierarchy

### Disable file caching
create `/var/www/html/.htaccess`

Add the following content in the file
```
#Initialize mod_rewrite
RewriteEngine On
<FilesMatch "\.(html|htm|js|css)$">
  FileETag None
  <IfModule mod_headers.c>
    Header unset ETag
    Header set Cache-Control "max-age=0, no-cache, no-store, must-revalidate"
    Header set Pragma "no-cache"
    Header set Expires "Wed, 12 Jan 1980 05:00:00 GMT"
  </IfModule>
</FilesMatch>
```

## With Custom Tool

### Rpi Server Background Workload
`./hackbench -s 10000 -g 1024 -i`

### Server machine background workload
`./hackbench -s 10000 -g 65536 -i`