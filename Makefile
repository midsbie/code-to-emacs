code: code.o

code.o: code.c

%o:
	gcc -O3 $< -o $>