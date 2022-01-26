NAME = rsa
CFLAGS = -O2 -Iinclude -lgmp -std=c11 -D_POSIX_C_SOURCE=200809L -Wall
SRC = src/rsalib.c \
			src/logs.c 	 \
			src/main.c	 
OBJ = $(SRC:.c=.o)

all: rsa
	
rsa: $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(CFLAGS) -lm

rsalib.o: src/rsalib.c 
main.o: 	src/main.c 
logs.o: 	src/logs.c

clean:
	-rm $(OBJ) $(NAME)
