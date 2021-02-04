# readme

## make

### USE_POLL

    make USER_DEFINES="-DUSE_POLL"

## test simultaneous open

- On host 172.17.0.2 run

      for i in {1..1000}; do ./a -b 172.17.0.2 10002 -c 172.17.0.3 10003 -r -w; done

- On host 172.17.0.3 run

      for i in {1..1000}; do ./a -b 172.17.0.3 10003 -c 172.17.0.2 10002 -r -w; done

