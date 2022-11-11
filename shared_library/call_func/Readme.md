# Readme

## Test

    $ ./call_func/run2 ./call_func/lib/libfoo2.so
    construct: D2
    ================================
    D2
    --------------------------------
    construct: D1
    lib ./call_func/lib/libfoo2.so loaded
    ================================
    D2
    D1
    --------------------------------
    deconstruct: D1
    deconstruct: D2

When libfoo2.so is loaded, `add_new` in binary file `run2` will be called, then `dlopen` will return.

## Note

In ./call_func/CMakeLists.txt, `add_link_options(-rdynamic)` is set before `add_executable(run2 main.cpp)`.

If not set link option `rdynamic`, error would occur as bellow:

    $ ./call_func/run2 ./call_func/lib/libfoo2.so 
    construct: D2
    ================================
    D2
    --------------------------------
    construct: D1
    ./call_func/run2: symbol lookup error: ./call_func/lib/libfoo2.so: undefined symbol: _Z7add_newP1B

Check symbol table:

    readelf -Ws --dyn-syms ./call_func/run2

`_Z7add_newP1B` only appear in `Symbol table '.symtab'`, not appear in `Symbol table '.dynsym'`

Further reading:

[shared object can't find symbols in main binary, C++](https://stackoverflow.com/questions/3623375/shared-object-cant-find-symbols-in-main-binary-c)

> Without the -rdynamic (or something equivalent, like -Wl,--export-dynamic), symbols from the application itself will not be available for dynamic linking.
