all:
	gcc ppmrw.c -o ppmrw
clean:
	rm -f ppmrw ppmrw.o
run: ./ppmrw
	./ppmrw