all: libhelpers.so libbufio.so

libhelpers.so: helpers.o
	gcc -shared -o libhelpers.so helpers.o

helpers.o: helpers.c
	gcc -fPIC -c -Wall helpers.c -o helpers.o

libbufio.so: bufio.o
	gcc -shared -o libbufio.so bufio.o

bufio.o: bufio.c
	gcc -fPIC -c -Wall bufio.c -o bufio.o

clean:
	rm -rf *.o *.so