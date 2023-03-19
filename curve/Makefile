all:;@:

build:
	mkdir -p build; \
	cd build; \
	cmake .. && \
	make -j 4

clean:
	rm -rf build/*

format:
	./format.sh

.PHONY: all build clean format
