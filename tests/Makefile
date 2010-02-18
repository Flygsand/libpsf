CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpsf

SRC=common.c test_psf2.c run_tests.c
OBJ=$(SRC:.c=.o)
BIN=run_tests

all: $(SRC) $(BIN)

$(BIN):	$(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

run: $(BIN)
	./$(BIN)
	
clean:
	-rm -f *.o $(BIN)
