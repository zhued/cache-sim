CC=gcc
CFLAGS=-Wall -g -O2
BIN=cache-sim
OBJ=main.o config.o

%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(BIN) : $(OBJ)
	$(CC) -o $(BIN) $(OBJ)

.PHONY : clean
clean :
	rm -rf $(OBJ) $(BIN)
