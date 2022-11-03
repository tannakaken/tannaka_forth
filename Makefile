forth: main.o stack.o machine.o interpreter.o
	gcc -o forth main.o stack.o machine.o interpreter.o -lreadline

main.o: main.c interpreter.h machine.h
	gcc -c main.c

stack.o: stack.c stack.h
	gcc -c stack.c

machine.o: machine.c stack.h machine.h
	gcc -c machine.c

interpreter.o: interpreter.c machine.h interpreter.h
	gcc -c interpreter.c

clean:
	rm *.o forth
