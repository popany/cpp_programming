CC=g++
CFLAGS=-I$(IDIR)

MKDIR_P = mkdir -p

IDIR=./include
ODIR=./obj
LDIR=./lib

LIBS=-lpthread

_DEPS= log.h utils.h config.h server.h connection.h socket_handler.h common_types.h http_request.h request_handler.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=log.o config.o server.o connection.o socket_handler.o http_request.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(LDIR)/%.cpp $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

all: directories a

.PHONY: directories

directories: ${ODIR}

${ODIR}:
	${MKDIR_P} ${ODIR}

a: main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

main.o: main.cpp $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f a $(ODIR)/*.o *~ core $(INCDIR)/*~ 
