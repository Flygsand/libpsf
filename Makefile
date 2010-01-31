CC=gcc -std=c99 -Wall
AR=ar
PREFIX=/usr

all: libpsf.c
	$(CC) -c -fPIC libpsf.c -o libpsf.o -lz
	$(CC) -shared -Wl,-soname,libpsf.so.1 -o libpsf.so.1.0.1 libpsf.o

install: all
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m755 libpsf.so.1.0.1 $(PREFIX)/lib
	install -m644 libpsf.h $(PREFIX)/include
	ln -sf $(PREFIX)/lib/libpsf.so.1.0.1 $(PREFIX)/lib/libpsf.so.1
	ln -sf $(PREFIX)/lib/libpsf.so.1.0.1 $(PREFIX)/lib/libpsf.so	

clean:
	-rm -f *.o *.so *.so.*
