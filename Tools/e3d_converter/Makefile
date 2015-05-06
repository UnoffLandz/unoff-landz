default: e3d_conv

CFLAGS= -O3 -W -Wall
LDFLAGS= -lssl -lcrypto -lm

OBJ= create_e3d.o files.o half.o main.o normal.o
e3d_conv: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

tags: *.h *.c
	ctags *.h *.c

clean:
	rm $(OBJ)
