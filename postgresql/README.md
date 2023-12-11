Requires `libpq`. This can be installed via `apt-get install libpq-dev`

## Setup database
Run
```
sudo -u postgres psql postgres
CREATE ROLE <username> LOGIN PASSWORD <password>;
CREATE DATABASE test WITH OWNER = <username>;
```

Edit `/etc/postgresql/*/main/postgresql.conf` and change line to `listen_addresses = '*'` also change line to `shared_buffers = 128kB`

Add the line `host  all  all 0.0.0.0/0 trust` to `/etc/postgresql/*/main/pg_hba.conf`
