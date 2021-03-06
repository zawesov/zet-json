PREFIX ?= /usr/local

CC=g++

CFLAGS= -g -static -Werror -Wno-reorder
LDPATH= 
LIBPATH = -L /usr/local/lib
LIBS= 
LIBNAME=
#TLOPT=/C /P64

OBJS=\
        zJSON.o


all: libzetjson.a $(OBJS)

clean:
	rm -rf $(OBJS) libzetjson.a

install: libzetjson.a
	cp -v ./zJSON.h $(PREFIX)/include/
	cp -v ./libzetjson.a $(PREFIX)/lib/

libzetjson.a : zJSON.o
	@rm -f ./libzetjson.a
	ar rcs ./libzetjson.a ./zJSON.o

zJSON.o: zJSON.cpp zJSON.h
	$(CC) $(CFLAGS) -c zJSON.cpp

