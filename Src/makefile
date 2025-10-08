CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL3 -lSDL3_image -lSDL3_mixer -lsqlite3
SRC = $(wildcard ./*.c)
OUT = ../game

build:
	gcc $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

run: 
	../game


clean:
	rm -f $(OUT)
