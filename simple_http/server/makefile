CC=g++
CFLAGS=-I$(IDIR)

IDIR=./include
ODIR=./obj
LDIR=./lib

LIBS=-lpthread

_DEPS= log.h utils.h config.h server.h connection.h socket_handler.h common_types.h http_request.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=log.o config.o server.o connection.o socket_handler.o http_request.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(LDIR)/%.cpp $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

a: main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

main.o: main.cpp $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f a $(ODIR)/*.o *~ core $(INCDIR)/*~ 
