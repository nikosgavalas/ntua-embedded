
def:
	@echo "make ex1|ex2|ex3"

ex1: ex1.s
	gcc -Wall ex1.s -o ex1

ex2: ex2.s
	gcc -Wall ex2.s -o ex2

ex3: ex3.c ex3.s
	gcc -Wall -g -c ex3.c -o ex3.o
	gcc -Wall -g -c ex3.s -o ex3_fun.o
	gcc -Wall ex3.o ex3_fun.o -o ex3
	rm -f *~ *.o
	gcc -Wall ex3_default.c -o ex3_default

clean:
	rm -f ex1 ex2 ex3 ex3_default out_file