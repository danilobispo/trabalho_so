# My first makefile

all: escalonador shutdown executa_postergado

escalonador: escalonador.o fattree.o torus.o global.h
	gcc -o escalonador fattree.o torus.o escalonador.o global.h


hipercubo: hipercubo.c
	gcc hipercubo.c -o hipercubo

torus.o: torus.c torus.h global.h
	gcc -c torus.c torus.h global.h

fattree.o: fattree.c fattree.h global.h
	gcc -c fattree.c fattree.h global.h

escalonador.o: escalonador.c escalonador.h global.h
	gcc -c escalonador.c escalonador.h global.h

shutdown: shutdown.c
	gcc -o shutdown shutdown.c 

executa_postergado: executa_postergado.c 
	gcc -o executa_postergado executa_postergado.c 

clean:
	rm -rf *.o *~ escalonador shutdown executa_postergado