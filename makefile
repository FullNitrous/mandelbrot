LIBS  = -lpng
CFLAGS = 

SRC=$(wildcard src/*.c)

bin/mandelbrot: $(SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)