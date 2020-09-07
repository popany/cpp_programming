all:
	echo "http_server"

clean:
	echo "clean http_server"

main.o: main.cpp $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

.PHONY: all 
.PHONY: clean 
