all: filesender

filesender: filesender.c
	gcc -L../lib -I../lib filesender.c -lbufio -lhelpers -o filesender  	

clean: 
	rm -f filesender
