all: libhelpers.so libbufio.so

libhelpers.so: helpers.o
	gcc -std=c99 -shared -o libhelpers.so helpers.o

helpers.o: helpers.c
	gcc -std=c99 -fPIC -c -Wall helpers.c -o helpers.o

libbufio.so: bufio.o
	gcc -std=c99 -shared -o libbufio.so bufio.o

bufio.o: bufio.c
	gcc -std=c99 -fPIC -c -Wall bufio.c -o bufio.o

clean:
	rm -rf *.o *.so