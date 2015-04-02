all:
	make -C lib/
	make -C cat/
	make -C revwords/
	make -C delwords/
	make -C filter/
	make -C bufcat/

clean:
	make clean -C lib/
	make clean -C cat/
	make clean -C revwords/
	make clean -C delwords/
	make clean -C filter/
	make clean -C bufcat/