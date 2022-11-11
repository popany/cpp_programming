# Readme

## Test

    $ ./static_init/run ./static_init/lib/libfoo.so 
    construct: D1
    lib ./static_init/lib/libfoo.so loaded
    deconstruct: D1

`static` variable `d1` is initialized before `dlopen` return.
