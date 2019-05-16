# My first makefile

# main: $(SYSTEMC_HOME) ula.o ula_tb.o main.o
# 	g++ -o ula ula.o ula_tb.o main.o -I. -I$(SYSTEMC_HOME)/include -L. -L$(SYSTEMC_HOME)/lib-macosx64 -lsystemc

main: fattree.o main.o
	gcc -o escalonador fattree.o main.o


fattree.o: fattree.c fattree.h
	gcc -c fattree.c fattree.h

# ula_tb.o: ula_tb.cpp ula_tb.h
# 	g++ -c ula_tb.cpp ula_tb.h -I. -I$(SYSTEMC_HOME)/include -L. -L$(SYSTEMC_HOME)/lib-macosx64 -lsystemc

main.o: main.c global.h
	gcc -c main.c global.h


clean:
	rm -rf *.o *~ main