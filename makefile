objects = main.o definition_func.o

stupidShell:$(objects)
	gcc -o stupidShell $(objects) -g -lpthread


main.o: stupidShell.h

definition.o: stupidShell.h


.PHONY:clean

clean:
	rm $(objects)
