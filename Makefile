CC=gcc -std=c99 -Wall
PREFIX=/usr

OBJECTS=psf.o psf2.o

all: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libpsf.so.1 -o libpsf.so.1.0.1 $(OBJECTS)

%.o: %.c	
	$(CC) -c -fPIC $< -o $@ -lz

install: all
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -d $(PREFIX)/include/psf
	install -m755 libpsf.so.1.0.1 $(PREFIX)/lib
	install -m644 *.h $(PREFIX)/include/psf
	ln -sf $(PREFIX)/lib/libpsf.so.1.0.1 $(PREFIX)/lib/libpsf.so.1
	ln -sf $(PREFIX)/lib/libpsf.so.1.0.1 $(PREFIX)/lib/libpsf.so	

clean:
	-rm -f *.o *.so *.so.*
