default: ul_conv

CFLAGS= -O3 -W -Wall -std=c99
LDFLAGS= -lssl -lcrypto -lm

OBJ= main.o e3d.o file.o half.o normal.o obj.o processing_options.o
ul_conv: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

tags: *.h *.c
	ctags *.h *.c

clean:
	rm $(OBJ)
