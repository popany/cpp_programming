CC=g++
CFLAGS=-I$(IDIR) -g

MKDIR_P = mkdir -p

IDIR=./include
ODIR=./obj
SRC=./src

LIBS=-lpthread

_DEPS=log.h utils.h server.h connection.h socket_handler.h common_types.h request_handler.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJS= server.o connection.o socket_handler.o
OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: directories tcp_server.a

directories: ${ODIR}

${ODIR}:
	${MKDIR_P} ${ODIR}

tcp_server.a: $(OBJS)
	ar rcs $@ $^

clean:
	rm -f tcp_server.a $(ODIR)/*.o *~ core $(INCDIR)/*~ 

.PHONY: all
.PHONY: directories
.PHONY: clean