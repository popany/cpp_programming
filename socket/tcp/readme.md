# readme

## make

### USE_POLL

    make USER_DEFINES="-DUSE_POLL"

## server

    ./a -l -a -b 0.0.0.0 10000 -r -w

## client

    ./a -c 127.0.0.1 10000 -r -w

## test simultaneous open

- On host 172.17.0.2 run

      for i in {1..1000}; do ./a -b 172.17.0.2 10002 -c 172.17.0.3 10003 -r -w; done

- On host 172.17.0.3 run

      for i in {1..1000}; do ./a -b 172.17.0.3 10003 -c 172.17.0.2 10002 -r -w; done

