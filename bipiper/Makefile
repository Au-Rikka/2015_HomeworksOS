all: forking polling

forking: forking.c
	gcc -L../lib -I../lib forking.c -lbufio -lhelpers -o forking  	

polling: polling.c
	gcc -L../lib -I../lib polling.c -lbufio -lhelpers -o polling  	

clean: 
	rm -f forking
	rm -f polling
