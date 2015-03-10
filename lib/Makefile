all: libhelpers.so

libhelpers.so: helpers.o
	gcc -shared -o libhelpers.so helpers.o

helpers.o: helpers.c
	gcc -c -Wall -fPIC helpers.c -o helpers.o

clean:
	rm -rf *.o *.so