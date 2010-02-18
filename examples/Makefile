CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpsf

SRC=psfxt.c
OBJ=$(SRC:.c=.o)
BIN=psfxt

all: $(SRC) $(BIN)

$(BIN):	$(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

run: $(BIN)
	./$(BIN)
	
clean:
	-rm -f *.o $(BIN)
